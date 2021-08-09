# Jammy's Old Infra

---
## Challenge info

### Description
Jammy asked me to build some login infrastructure for his website. Well, here it is.

### Author
Tango

### Category
Reverse Engineering

### Flag
```
rarctf{pl34s3_d0nt_s0lv3_st4tic4lly_3829103890}
```

### Time spent developing and exploiting
21 days (screw you QBDI)

---

## TL;DR

1. Decompress apk, read AndroidManifest.xml which says it's usable on Android 11.

2. Install app on an emulator and run. App asks for a username and password.

3. Read source code from decompressed apk, see it performs checks within native code (C++ using JNI)

4. Try and reverse C++, see it's too messy to reverse without spending too much time on it

5. Solve dynamically, creating a frida script and hooking into the functions

6. See username and password in memory

7. Log in with credentials

8. ???

9. Flag

---

## Writeup

### Contents
- [Info Gathering](#info-gathering)
- [Running the app](#running-the-app)
- [Code analysis](#code-analysis)
- [Reversing native code](#reversing-native-code)
- [Dynamically solving](#dynamically-solve)
	- [Starting frida](#starting-frida)
	- [Creating a simple script](#creating-a-simple-script)
	- [Hooking native functions](#hooking-native-functions)
- [Flagging](#flagging)

## Info gathering
[Jump back to the top](#writeup)

We're given a singular apk file, so the first thing to always do is check for low hanging fruit:

- Running `strings` on the apk, searching for the flag
	- No luck
	```bash
	$ strings jammys_old_infra.apk | grep rarctf
	$
	```

I can't think of anymore low hanging fruit, so time to decompress the app (remembering that an `apk` is just a zip file).
```bash
$ file jammys_old_infra.apk
jammys_old_infra.apk: Zip archive data, at least v0.0 to extract, compression method=deflate
```
We can use `apktool` to do this, as this should also convert and bytecode to normal `java` source files. The command is `apktool d app-name.apk`:

```bash
$ apktool --help
Unrecognized option: --help
Apktool v2.5.0 - a tool for reengineering Android apk files
with smali v2.4.0 and baksmali v2.4.0
Copyright 2010 Ryszard Wiśniewski brut.alll@gmail.com
Copyright 2010 Connor Tumbleson connor.tumbleson@gmail.com
usage: apktool
...
usage: apktool d[ecode] [options] file_apk
 -f,--force              Force delete destination directory.
 -o,--output dir       The name of folder that gets written. Default is apk.out
 -p,--frame-path dir   Uses framework files located in dir.
 -r,--no-res             Do not decode resources.
 -s,--no-src             Do not decode sources.
 -t,--frame-tag tag    Uses framework files tagged by tag.
...
╭─tango@arch joi
╰─$ apktool d jammys_old_infra.apk 
I: Using Apktool 2.5.0 on jammys_old_infra.apk
I: Loading resource table...
I: Decoding AndroidManifest.xml with resources...
I: Loading resource table from file: /home/tango/.local/share/apktool/framework/1.apk
I: Regular manifest package...
I: Decoding file-resources...
I: Decoding values */* XMLs...
I: Baksmaling classes.dex...
I: Copying assets and libs...
I: Copying unknown files...
I: Copying original files...
╭─tango@arch joi 
╰─$ ls -l jammys_old_infra 
total 24
-rw-r--r--   1 tango tango 1057 Jul 27 16:52 AndroidManifest.xml
-rw-r--r--   1 tango tango 2417 Jul 27 16:52 apktool.yml
drwxr-xr-x   6 tango tango 4096 Jul 27 16:52 lib
drwxr-xr-x   3 tango tango 4096 Jul 27 16:52 original
drwxr-xr-x 137 tango tango 4096 Jul 27 16:52 res
drwxr-xr-x   5 tango tango 4096 Jul 27 16:52 smali
╭─tango@arch solve ‹master*›
╰─$ cd jammys_old_infra
```

Now to start exploring the app. As a reminder of how an android app is layed out:

```bash
.
├── AndroidManifest.xml			# Configurations for the app
├── apktool.yml					# apktool own file, not included in original src
├── lib							# Any native code, developed in C/C++
│   ├── arm64-v8a			  	# Same file, different architectures
│   │   └── libnative-lib.so
│   ├── armeabi-v7a
│   │   └── libnative-lib.so
│   ├── x86
│   │   └── libnative-lib.so
│   └── x86_64
│       └── libnative-lib.so
├── original
│   ├── AndroidManifest.xml		# Original, non-human readable file
│   └── META-INF				   # Apk signatures
│       ├── ...
├── res							# Resources file, mainly containing xml files
│   ├── ...
└── smali						  # Java bytecode files
    ├── android
    │   └── support
    │       └── v4
    │           ├── ...
    ├── androidx
    │   ├── ...
    └── com
        ├── example
        │   └── jammys_old_infra   # Smali code of this apk
        │       ├── ...
        └── google
            └── android
                └── material
                    ├── ...

323 directories, 3953 files
```

There's a lot of files we don't care about. The ones which we do are:
- **AndroidManifest.xml**
- **lib/**
- **smali/com/example/jammys_old_infra/**

Our first thing to always take note is the `AndroidManifest.xml` file. This contains the configurations for the android app, such as what Android OS it was built against, what permissions the app needs to use, the name of the app, etc.

```xml
<?xml version="1.0" encoding="utf-8" standalone="no"?>
<manifest
	xmlns:android="http://schemas.android.com/apk/res/android"
	android:compileSdkVersion="30"
	android:compileSdkVersionCodename="11"
	package="com.example.jammys_old_infra"
	platformBuildVersionCode="30"
	platformBuildVersionName="11">
    <uses-permission android:name="android.permission.INTERNET"/>
    <application
		android:allowBackup="true"
		android:appComponentFactory="androidx.core.app.CoreComponentFactory"
		android:extractNativeLibs="false"
		android:icon="@mipmap/ic_launcher"
		android:label="@string/app_name"
		android:roundIcon="@mipmap/ic_launcher_round"
		android:supportsRtl="true"
		android:theme="@style/Theme.Jammys_old_infra"
		android:usesCleartextTraffic="true">
        <activity android:name="com.example.jammys_old_infra.MainActivity">
            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
            </intent-filter>
        </activity>
    </application>
</manifest>
```

It says that has been compiled against SDK Version 30, which is Android 11. Unfortunately, no target SDK version has been given, so I'll be using Android 11.

The application then starts. There's nothing of note in the application tag, just that we're most likely going to be dealing with a webpage than requests HTTP pages (`usesCleartextTraffic="true"` and the application requests the `INTERNET` permission).

Inside the application, the first `activity` is `com.example.jammys_old_infra.MainActivity`.  Let's try and find that out:

```bash
$ find ./ -name 'MainActivity' -type f 2>/dev/null
╭─tango@arch joi
╰─$ find ./ -name '*MainActivity*' -type f 2>/dev/null
./smali/com/example/jammys_old_infra/MainActivity$1.smali
./smali/com/example/jammys_old_infra/-$$Lambda$MainActivity$1hiwpDhtFzpSLMJkxk7bDg9bBJE.smali
./smali/com/example/jammys_old_infra/MainActivity.smali
./smali/com/example/jammys_old_infra/-$$Lambda$MainActivity$Q0h81coz-AuZg1gEg7dAr-3PLTM.smali
./smali/com/example/jammys_old_infra/MainActivity$1$1.smali
./smali/com/example/jammys_old_infra/MainActivity$2.smali
```

Unfortunately, the file `smali/com/example/jammys_old_infra/MainActivity.smali` is in smali, which is Java bytecode:
```Java
$ head -n 20 smali/com/example/jammys_old_infra/MainActivity.smali
.class public Lcom/example/jammys_old_infra/MainActivity;
.super Landroidx/appcompat/app/AppCompatActivity;
.source "MainActivity.java"


# static fields
.field private static final WEBSITE:Ljava/lang/String; = "http://ec2-18-130-135-40.eu-west-2.compute.amazonaws.com:8090"


# direct methods
.method static constructor <clinit>()V
    .locals 1

    const-string v0, "native-lib"

    .line 30
    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V

    return-void
.end method
```

Which is a lot harder to read than Java. To put this into what the code would be:
```java
public class MainActivity extends AppCompatActivity {

	private static final String WEBSITE = "http://ec2-18-130-135-40.eu-west-2.compute.amazonaws.com:8090";

    static {
        System.loadLibrary("native-lib");
    }
```

So to convert the smali to java:
[StackOverflow post](https://stackoverflow.com/questions/5582824/decompile-smali-files-on-an-apk)

Which says that we can\'t turn the bytecode back to the source code.

Now that we\'ve taken a quick look at the app, let\'s run it.

### Running the app
[Jump back to the top](#writeup)

First, let's create an emulator for this, which runs Android 11. We can either do this by downloading the Android SDK, or using `genymotion`. Because I've already downloaded the android SDK, I'm going to use that.

(The Android SDK is installed in my home directory, in `~/Android/SDK`).

Let's create an emulator!
We can either use Android-studio to generate it, or we can use `avdmanager` (Android Device Manager). Let's go through `android studio` because setting it up through the command line is messy.

1. Open android studio (if you're already in a project, just go file -> close project)
![Android studio main page](./images/android-studio-main-page.png)

2. Click on `configure` -> `AVD manager`
3. Create a new virtual device (bottom left)
4. Select any type of android phone
	- I'm going to use one without google play installed, as that's a rooted device (easier to exploit this application and use frida, which will be used later) ([See this StackOverflow answer about this](https://stackoverflow.com/a/67172015))
	- I've selected the `Pixel 3a XL`
5. Select the API version 30
	- You might have to download the firmware first before proceeding
6. Configure the machine however you want
	- I've left the defaults on

Now, we can start it up. If we wanted to start it up from the command line, we would do the following:

```bash
$ cd ~/Android/Sdk # my android Sdk installation path
$ emulator/emulator -avd jammys_old_infra
```
Once it's started up, we can *install* the application onto the device via adb:
```bash
$ adb devices
List of devices attached
emulator-5554	device
$ adb -s emulator-5554 install /path/to/jammys_old_infra.apk
Performing Streamed Install
Success
```
(The `-s` flag just means which serial device to use. This is used when you have multiple adb devices connected, such as your phone and an emulator).

And now we can click up on the emulator, and double click the new application `Janes New Infra`.

![Installed the apk onto the phone](./images/installed-apk.png)

![Opened the apk](./images/opened-apk.png)

So it looks to be a login page. If we enter some random data `test:pass` ...

![Incorrect details](./images/apk-incorrect-deets.png)

And clicking on the `Would you like to retry?` button brings us back to the login page.

So, the idea is most likely to find the username and password, allowing us to login.

### Code analysis
[Jump back to the top](#writeup)

If we go back to the apk, we can use the tool `jadx` to view the original java source code. I'll use `jadx-gui` for easily viewing the code. If we import the file into jadx-gui, expand the file path `com/example.jammys_old_infra` and double click on `MainActivity`

![File structure for MainActivity file](./images/open_mainActivity.png)

Brings up the source code.

The first executing function is `onCreate` (read [the Android docs](https://developer.android.com/guide/components/activities/activity-lifecycle) to understand the reasons for that).

Which is as follows:
```java
@Override // androidx.activity.ComponentActivity, androidx.core.app.ComponentActivity, androidx.fragment.app.FragmentActivity
public void onCreate(Bundle bundle) {
    super.onCreate(bundle);
    setContentView(R.layout.viewswitcher);
    main();
}
```

It calls the original `onCreate` function, and then sets the content displayed to the screen (this is all normal execution for an android app).

After, it calls the function `main` (which isn't normal execution). What does this do?
```java
private void main() {
    final ViewSwitcher viewSwitcher = (ViewSwitcher) findViewById(R.id.viewswitcher);
    WebView webView = (WebView) findViewById(R.id.webview);
    webView.getSettings().setJavaScriptEnabled(true);
    webView.setWebViewClient(new WebViewClient() {
        /* class com.example.jammys_old_infra.MainActivity.AnonymousClass3 */

       @Override // android.webkit.WebViewClient
       public WebResourceResponse shouldInterceptRequest(WebView webView, final WebResourceRequest webResourceRequest) {
            new Thread(new Runnable() {
                /* class com.example.jammys_old_infra.MainActivity.AnonymousClass3.AnonymousClass1 */

                public void run() {
                    String str;
                    String valueOf = String.valueOf(webResourceRequest.getUrl().getEncodedQuery());
                    List asList = Arrays.asList(valueOf.split("="));
                    if (asList.size() > 1 && !valueOf.equals("v=4.7.0")) {
                        String str2 = null;
                        try {
                            str = URLDecoder.decode(((String) asList.get(asList.size() - 1)).split("&")[0], StandardCharsets.UTF_8.toString());
                        } catch (UnsupportedEncodingException e) {
                            e.printStackTrace();
                            str = null;
                        }
                        try {
                            str2 = URLDecoder.decode(((String) asList.get(1)).split("&")[0], StandardCharsets.UTF_8.toString());
                        } catch (UnsupportedEncodingException e2) {
                            e2.printStackTrace();
                        }
                        MainActivity.this.checkCredentialsUserProvides(viewSwitcher, str2, str);
                    }
                }
            }).start();
            return super.shouldInterceptRequest(webView, webResourceRequest);
        }
    });
    webView.loadUrl(WEBSITE);
}
```

The boring stuff is that it just sets up a [WebView](https://developer.android.com/reference/android/webkit/WebView) at the start of the function, and then loads the website at the end of the function.

The website in question is defined at the top of the class
```java
private static final String WEBSITE = "http://ec2-18-130-135-40.eu-west-2.compute.amazonaws.com:8090";
```

The interesting bit is the overridden function `shouldInterceptRequest`. On a new thread ([good coding practice because UI thread for the main UI stuff](https://developer.android.com/topic/performance/threads)), the values of the URL are stored in a variable.

A check is performed to make sure that they size of the variable actually has some data in it (greater than 1), and that it doesn't equal a certain string.
If that's the case, then the last element of the URL is got, and is split on the `&` character. If this isn't possible, then a stack trace is shown.

The same is done for the variable `str2`, apart from getting the first element in the URL.

These two values are then passed into the function `checkCredentialsUserProvides`.
```java
private void checkCredentialsUserProvides(final ViewSwitcher viewSwitcher, String str, String str2) {
    boolean checkCreds;
    if (checkCreds(str, str2, 1) && checkCreds(str, str2, 0)) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            /* class com.example.jammys_old_infra.MainActivity.AnonymousClass1 */

            public void run() {
                Toast.makeText(MainActivity.this.getApplicationContext(), "Well done, you've got the flag!", 0).show();
            }
        });
    } else {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            /* class com.example.jammys_old_infra.MainActivity.AnonymousClass2 */

            public void run() {
                viewSwitcher.showNext();
                ((Button) MainActivity.this.findViewById(R.id.button)).setOnClickListener(new View.OnClickListener() {
                    /* class com.example.jammys_old_infra.MainActivity.AnonymousClass2.AnonymousClass1 */

                    public void onClick(View view) {
                       viewSwitcher.showNext();
                    }
                });
            }
        });
    }
}
```
The variables `str1` and `str2` are passed into the function `checkCreds`, as well as an integer. If both calls are true, then a [toast notification](https://developer.android.com/guide/topics/ui/notifiers/toasts) pops up, saying that you've got the flag.

If either one of the checks fail, then the [view](https://developer.android.com/reference/android/view/View?hl=en) (the way the contents are displayed) changes, shows a button, and waits for the user to click it.

So what does `checkCreds` do?
```java
public native boolean checkCreds(String str, String str2, int i);

static {
    System.loadLibrary("native-lib");
}
```
So it's a native function, which is defined the the library `native-lib.so`.

Searching the internet for what the `native` keyword means, brings up [the android NDK docs](https://developer.android.com/ndk)
> The Android NDK is a toolset that lets you implement parts of your app in native code, using languages such as C and C++

So to understand what `checkCreds` is doing, we've got to reverse some C/C++ code.

### Reversing native code
[Jump back to the top](#writeup)

> As a preface, I would recommend learning about the basics of how C/C++ code can interact with Java code, by utilising the Java Native Interface (JNI). These links should get you started:
>
> - https://www.ragingrock.com/AndroidAppRE/reversing_native_libs.html
>
> - https://developer.android.com/ndk/guides
>
> - https://blog.mindorks.com/getting-started-with-android-ndk-android-tutorial

> I've tried to explain what most JNI functions do, but tired 2am me might not have been so sure on that

Because I've mainly analysed x86 code, I'm going to reverse the x86 binary with `ghidra` (it's also the architecture that my emulator runs on, so it'll make it easier when trying to debug and exploit).

To see what we're dealing with, we're going to run the command `file` against the binary:
```bash
$ file lib/x86/libnative-lib.so
lib/x86/libnative-lib.so: ELF 32-bit LSB shared object, Intel 80386, version 1 (SYSV), dynamically linked, BuildID[sha1]=fbabc7d3f25ccb9a25045c7e256b56fc70d1909c, stripped
```

A linux binary, dynamically linked, and stripped.

Opening the file up in the ghidra, going to the code browser and expanding the `functions` tab in the symbol tree:
![Ghidra functions](./images/ghidra_functions.png)

We don't get our usual `entry` function, as with normal ELF binaries.

If we search on the internet `reverse android native binary`, the first link is [Maddie Stone's blog](https://www.ragingrock.com/AndroidAppRE/reversing_native_libs.html) (I would check out all of her exercises, they're a really good starting point to android reverse engineering).

In the section ***## Reversing Android Native Libraries Code - JNIEnv***, Maddie tells us

> Thankfully, there’s a way to get the JNI [(Java Native Interface)] function[s] without [detecting what JNI function does what] manually! In both the Ghidra and IDA Pro decompilers you can re-type the first argument in JNI functions to `JNIEnv *` type and it will automatically identify the JNI Functions being called. In IDA Pro, this work[s] out of the box. In Ghidra, you have to load the JNI types (either the jni.h file or a Ghidra Data Types archive of the jni.h file) first. For ease, we will load the JNI types from the Ghidra Data Types archive (gdt) produced by Ayrx and available [here](https://github.com/Ayrx/JNIAnalyzer/blob/master/JNIAnalyzer/data/jni_all.gdt).

If we download the file, and follow Maddie's steps to import it into ghidra, we can now analyse the JNI functions.

The function that we want to analyse is the `checkCreds` function. This is actually known as `Java_com_example_jammys_1old_1infra_MainActivity_checkCreds`, which follows the pattern `Java_name-of-app_file-imported-from_function-name`.

Opening that up:
```c++
undefined4
Java_com_example_jammys_1old_1infra_MainActivity_checkCreds(
			_JNIEnv *param_1,
		  	undefined4 param_2,
		  	_jstring *param_3,
		  	_jstring *param_4,
		  	int param_5
		)

{
  undefined4 uVar1;
  _jobject *in_stack_ffffffe8;

  uVar1 = 0;
  if ((param_3 != (_jstring *)0x0) && (param_4 != (_jstring *)0x0)) {
    if (param_5 == 1) {
      uVar1 = checkPasswords(param_1,param_4);
    }
    else {
      if (param_5 == 0) {
        uVar1 = checkUsername(param_1,in_stack_ffffffe8,param_3);
      }
      else {
        uVar1 = 0;
      }
    }
  }
  return uVar1;
}
```
(Notice that the first parameter has automatically been retyped to `JNIEnv *param1`, and some of the variables have been retyped to their respect `j[type]`)

When dealing with native functions, the general parameters are as follows (these are the default ones when creating a new native project in `android-studio`)

```c++
Java_package_file_function(
	JNIEnv *env,
	jobject /* this */
) { ... }
```

So that means that any parameters after `param_2` must be user defined. Therefore, we can rename our parameters:

-> `param_3` = `str1_J`

->  `param_4` = `str2_J` 

-> `param_5` = `i_J`

(`_J` = from Java)

First, the function checks whether `str1_J` or `str2_J` are null.

If `i_J` is 1, then the program will enter the function `checkPasswords`, with parameters of the `JNIEnv*`, and `str2_J`.
If `i_J` is 0, then the program enters the function `checkUsernames` with parameters `JNIEnv*`, something from the stack, and `str1_J`.

Let's jump into the `checkPasswords` function.
```c++

/* checkPasswords(_JNIEnv*, _jstring*) */

bool checkPasswords(_JNIEnv *param_1,_jstring *param_2)

{
  byte *__s2;
  char *pcVar1;
  int iVar2;
  uint uVar3;
  byte bVar4;
  int in_GS_OFFSET;
  bool bVar5;
  byte *pbVar6;
  byte local_34;
  byte abStack51 [3];
  uint local_30;
  byte *pbStack44;
  basic_string<char,std::__ndk1::char_traits<char>,std::__ndk1::allocator<char>> local_24;
  byte abStack35 [3];
  size_t local_20;
  byte *pbStack28;
  int local_14;

  local_14 = *(int *)(in_GS_OFFSET + 0x14);
  pcVar1 = (char *)(**(code **)(*(int *)param_1 + 0x2a4))(param_1,param_2,0);
  std::__ndk1::basic_string<char,std::__ndk1::char_traits<char>,std::__ndk1::allocator<char>>::
  basic_string<decltype(nullptr)>(&local_24,pcVar1);
                    /* try { // try from 0001904d to 0001905b has its CatchHandler @ 0001915f */
  pbVar6 = &local_34;
  aes_cbc_decrypt();
  bVar4 = (byte)local_24 & 1;
  uVar3 = (uint)((byte)local_24 >> 1);
  if (bVar4 != 0) {
    uVar3 = local_20;
  }
  if ((local_34 & 1) == 0) {
    local_30 = (uint)(local_34 >> 1);
  }
  if (uVar3 == local_30) {
    __s2 = pbStack44;
    if ((local_34 & 1) == 0) {
      __s2 = abStack51;
    }
    if (bVar4 == 0) {
      bVar5 = true;
      if (uVar3 != 0) {
        uVar3 = 0;
        do {
          if (abStack35[uVar3] != __s2[uVar3]) goto LAB_00019105;
          uVar3 = uVar3 + 1;
        } while ((byte)local_24 >> 1 != uVar3);
      }
    }
    else {
      if (uVar3 == 0) {
        bVar5 = true;
      }
      else {
        pbVar6 = pbStack28;
        iVar2 = memcmp(pbStack28,__s2,uVar3);
        bVar5 = iVar2 == 0;
      }
    }
  }
  else {
LAB_00019105:
    bVar5 = false;
  }
  if ((local_34 & 1) != 0) {
    operator.delete(pbStack44);
    bVar4 = (byte)local_24 & 1;
    pbVar6 = pbStack44;
  }
  if (bVar4 != 0) {
    operator.delete(pbStack28);
    pbVar6 = pbStack28;
  }
  if (*(int *)(in_GS_OFFSET + 0x14) == local_14) {
    return bVar5;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail(pbVar6);
}
```

Ah, so it looks like we're dealing with C++, as `std::__ndk1::basic_string<char,std::__ndk1::char_traits<char>,std::__ndk1::allocator<char>>::
  basic_string<decltype(nullptr)>(&local_24,pcVar1);` is the equivalent of `std::string local_24`.

Following the code, the function `aes_cbc_decrypt` is called. Let's view this .... anddddd that's a lot of code ....

***Pausing thought***

There's probably a lot of code for a reason. And, from the name of the function, `aes_cbc_decrypt`, it looks as though the function is decrypting an AES value. As you can't decrypt AES without its key, would there be any point in statically reversing this binary?

Probably not. So, how else could you reverse this? We've tried statically reversing it, so let's try dynamically solving.

### Dynamically solve
[Jump back to the top](#writeup)

How could we solve this dynamically? That would require running the app, hooking it with a tool, looking at the memory and trying to find the plain text values.

(This is what you would normally do with binary exploitation - running the app, attaching a debugger to the process, and looking at the memory).

 We can do the exact same thing (pretty much) with a selection of tools:
 - [Frida](https://frida.re)
 	- [R2Frida](https://github.com/nowsecure/r2frida)
 - [QBDI](https://qbdi.quarkslab.com)
 - [Pin](https://software.intel.com/content/www/us/en/develop/articles/pin-a-dynamic-binary-instrumentation-tool.html)
 - [Valgrind](https://www.valgrind.org)
 - [DynamoRIO](https://dynamorio.org)

There might be a couple more, I can't remember.

I initially tried using frida to complete this, and reading through [this blog by quarkslab](https://blog.quarkslab.com/why-are-frida-and-qbdi-a-great-blend-on-android.html) :

> Using this [frida] script, we can solely access the inputs (arguments) and output (return value) of the function — that is, we're at the function layer. It's quite limiting and this alone is basically not enough to accurately grasp what's going on inside. Therefore, in this precise situation, we would like to inspect the function thoroughly, at a lower level.
>
> **Frida + QBDI**
>
> Importing features offered by QBDI can help us overcome this issue.

So I started looking at QBDI and trying to implement the tool, however, there isn't much usage of the tool on the public internet.

On the other hand, there is a lot of usage about frida, and when I asked for help solving this, someone helped me with using it. So, I shall use `frida` to solve this.

Let's start with attaching a simple frida script to the app.

#### Starting frida
[Jump back to the top](#writeup)

There's a bit of setting up to do with frida.
We will follow the [docs](https://frida.re/docs) to get started.

Frida requires a server on the device, which our frida "hooking" scripts will interact with. In our case, there will be a `frida-server` on the android, and our PC will be the "client".

Following https://frida.re/docs/android/, we will install `frida-server` on to the android device

> ***Frida does require a rooted device to run. If you're running this on a non-rooted device, you can follow [frida-gadget](https://frida.re/docs/gadget), which acts as `frida-server`, although is injected into the apk itself, rather than being system-wide. Steps to installing and using are here:***
> .
> https://koz.io/using-frida-on-android-without-root.
> .
>  ***Or, if you want to easily install it without having to mess with any code, follow these instructions:***
> .
> https://mobile-security.gitbook.io/mobile-security-testing-guide/android-testing-guide/0x05c-reverse-engineering-and-tampering#dynamic-analysis-on-non-rooted-devices.
> .
> ***When using `objection`, you'll have to use the `--gadget` flag, with the parameter as the name of the app***
> .
> `$ objection --gadget com.example.jammys_old_infra explore`

The following steps are how I have installed the server on to my emulator (if using frida-gadget, download the latest frida-gadget for android)

```bash
╭─tango@arch exploit
╰─$ wget https://github.com/frida/frida/releases/download/14.2.18/frida-server-14.2.18-android-x86.xz

--2021-07-02 13:31:18--  https://github.com/frida/frida/releases/download/14.2.18/frida-server-14.2.18-android-x86.xz
Loaded CA certificate '/etc/ssl/certs/ca-certificates.crt'
Resolving github.com (github.com)... 140.82.121.3
Connecting to github.com (github.com)|140.82.121.3|:443... connected.
HTTP request sent, awaiting response... 302 Found
Location: https://github-releases.githubusercontent.com/9405122/d4844b00-afa0-11eb-99f6-e4862b7d9492?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20210702%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20210702T123119Z&X-Amz-Expires=300&X-Amz-Signature=82006f1ac9820294f9433f9e76e281f2c6d2478357108f82f946b708b7ba4681&X-Amz-SignedHeaders=host&actor_id=0&key_id=0&repo_id=9405122&response-content-disposition=attachment%3B%20filename%3Dfrida-server-14.2.18-android-x86.xz&response-content-type=application%2Foctet-stream [following]
--2021-07-02 13:31:19--  https://github-releases.githubusercontent.com/9405122/d4844b00-afa0-11eb-99f6-e4862b7d9492?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20210702%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20210702T123119Z&X-Amz-Expires=300&X-Amz-Signature=82006f1ac9820294f9433f9e76e281f2c6d2478357108f82f946b708b7ba4681&X-Amz-SignedHeaders=host&actor_id=0&key_id=0&repo_id=9405122&response-content-disposition=attachment%3B%20filename%3Dfrida-server-14.2.18-android-x86.xz&response-content-type=application%2Foctet-stream
Resolving github-releases.githubusercontent.com (github-releases.githubusercontent.com)... 185.199.110.154, 185.199.111.154, 185.199.109.154, ...
Connecting to github-releases.githubusercontent.com (github-releases.githubusercontent.com)|185.199.110.154|:443... connected.
HTTP request sent, awaiting response... 200 OK
Length: 13376624 (13M) [application/octet-stream]
Saving to: ‘frida-server-14.2.18-android-x86.xz’

frida-server-14.2.18-android-x86.xz                        100%[=======================================================================================================================================>]  12.76M  5.34MB/s    in 2.4s    

2021-07-02 13:31:23 (5.34 MB/s) - ‘frida-server-14.2.18-android-x86.xz’ saved [13376624/13376624]

╭─tango@arch exploit
╰─$ ls
frida-server-14.2.18-android-x86.xz

╭─tango@arch exploit
╰─$ unxz frida-server-14.2.18-android-x86.xz

╭─tango@arch exploit
╰─$ ls
frida-server-14.2.18-android-x86
```
On a new terminal, start the emulator:
```bash
$ ~/Android/Sdk/emulator/emulator -avd Pixel_3a_XL_API_30
```
Back to the original terminal (if using `frida-gadget`, ignore these steps):

```bash
╭─tango@arch exploit
╰─$ adb devices
* daemon not running; starting now at tcp:5037
* daemon started successfully
List of devices attached
emulator-5554	offline

╭─tango@arch exploit
╰─$ adb devices
List of devices attached
emulator-5554	device

╭─tango@arch exploit
╰─$ adb -s emulator-5554 root
restarting adbd as root

╭─tango@arch exploit
╰─$ adb -s emulator-5554 push frida-server-14.2.18-android-x86 /data/local/tmp/frida-server
frida-server-14.2.18-android-x86: 1 file pushed, 0 skipped. 0.3 MB/s (42958488 bytes in 127.364s)

╭─tango@arch exploit
╰─$ adb -s emulator-5554 shell chmod +x /data/local/tmp/frida-server

# (If the device goes offline, just wait a couple of seconds.
# It does take quite a lot of RAM usage unfortunately).

╭─tango@arch exploit
╰─$ adb -s emulator-5554 shell '/data/local/tmp/frida-server'
```

And now, let's run a quick test with `frida-ps` (included in the `frida-tools` package - to use this, install the package with pip - `pip install frida-tools`)

(`frida-gadget` people, you can come back now)

```bash
$ frida-ps -U
 PID  Name
----  -------------------------------------------------------------
4128  adbd
 199  android.hardware.atrace@1.0-service
 ...
```

(The `-U` option means to track the USB device's processes)

Woohoo, frida has installed and is working fine.

#### Creating a simple script
[Jump to the top](#writeup)

Now that frida is working, let's create a simple script to interact with the application.

We can create a python script which starts the application, and injects our own script into it (I've named this `spawn.py`):
```python
import frida, sys, time

app_name = 'com.example.jammys_old_infra'
device = frida.get_usb_device() # get active devices, including emulators
pid = device.spawn(app_name) # spawn the application
device.resume(pid) # resume normal action
time.sleep(1)

print('[*] Attaching to process')
process = device.attach(pid) # attach to the process
jscode = open('hook_test.js').read() # read the javascript used to hook
script = process.create_script(jscode)
script.load() # load it into the application
sys.stdin.read()
```

You don't need to have this file, you can inject it from the command line (with the app already running):
```bash
$ frida -U -l hook.js com.example.jammys_old_infra
```
Where the `-U` means to use a USB device, the `-l` means what script to execute.

Or, inject the file without the app needed to be ran:
```bash
$ frida -U -l hook.js -f com.example.jammys_old_infra --no-pause
```
Where `-f com.example.jammys_old_infra` spawns the app, and `--no-pause` starts the app without it having to wait for a debugger to attach first.

> If, when running the python script, you come up with the "This app is waiting for a debugger", follow these steps:
> 

> 1. Go to settings
> 2. About emulated phone
> 3. Scroll down to the build number
> 4. Tap the build number 7 times to become a developer
> 5. Go back, system, advanced options, open developer options
> 6. Scroll down until you see the "Wait for debugger" option (in the debugging section)
> 7. Untick it
> 8. You should be able to now attach without the app waiting for a debugger

And `hook.js` contains the following:
```javascript
// perform the following Java function (remember hooking to android, so we use Java)
Java.perform(function() {
	console.log("[*] Starting hooking");
	 // define the activity to hook to
	var Activity = Java.use("com.example.jammys_old_infra.MainActivity");
	// Override the current implementation
	Activity.checkCredentialsUserProvides.implementation = function(vSwitch, username, pass) {
		console.log("Username: " + username);
		console.log("Password: " + pass);
		// perform normal operations, using the keyword "this"
		return this.checkCredentialsUserProvides(vSwitch, username, pass);
	}
});
```
If you remember, the `checkCredentialsUserProvides` is the java function which then goes into the native function `checkCreds`, and then displays the toast notification that we got the correct credentials, or displays the error view.

So now, running the python script:

```bash
$ python spawn.py
[*] Attaching to process
[*] Starting hooking
# Go to emulator, enter some fake data
Username: hey
Password: AAAAAAAA
# See that emulator returns error view
```

Great, we're now able to hook into java functions. Now, how about hooking into native functions?

#### Hooking native functions
[Jump back to the top](#writeup)

The frida javascript API provides the module `Interceptor` which allows us to intercept functions, and see the values being passed when entering it (`onEnter`), and when the application leaves the function (`onLeave`).

Replacing the `hook_test.js` file with `hook_native.js` in the python file, we can implement the Interceptor module.

I'm going to follow [this blog](https://erev0s.com/blog/how-hook-android-native-methods-frida-noob-friendly). We can attach to the function we initially go into, `Java_com_example_jammys_1old_1infra_MainActivity_checkCreds`, as found in the ghidra function list.

```javascript
console.log('[*] Starting Intercept');
Interceptor.attach(Module.getExportByName('libnative-lib.so',
 'Java_com_example_jammys_1old_1infra_MainActivity_checkCreds'), {
    onEnter: function (args) {
			console.log("inside checkCreds function");
	},
    onLeave: function(retval) {
			console.log("outside checkCreds");
	}
});
```
The script is pretty much as it says on the tin - it attaches to the exported function, and when it has been entered, logs the message. When the app is leaving the function, it'll log the message inside the function.

And now running it:
```bash
[*] Attaching to process
[*] Starting Intercept
inside checkCreds function
outside checkCreds
inside checkCreds function
outside checkCreds
```

It executes twice because it checks both the username and the password, and does two separate calls.

So, how do we turn this into something more useful?

We can assume that the function `aes_cbc_decrypt()` in `libnative-lib.so` decrypts the password, and stores it in memory. So, let's attach to that function, and see what it includes.

Instead of attaching via the function name, we're going to attach via a memory location. This can be done by finding the base address of the library (because ASLR), and then adding the offset of where the function is. Ghidra loads the binary at the address `0x00010000`, and then when we go to `aes_cbc_decrypt()`, it is at the address `0x000185d0`. Finding the offset of the function is simply:
```bash
$ python -c 'print(hex(0x000185d0 - 0x00010000))'
0x85d0
```

And then, when we leave the function, we're going to print a memory dump, to see what is there when the function ends.

```javascript
function wait_for_lib_load() {
	var remaining = 2;
	if (--remaining > 0) {
		setTimeout(intercept, 5000);
	}
}

function intercept() {

	wait_for_lib_load();
	console.log('[*] Starting Interceptor');
	Interceptor.attach(Module.findBaseAddress('libnative-lib.so').add(0x85d0), {
	    onEnter: function (args) {
				console.log("inside decrypt function");
	    },
	    onLeave: function(retval) {
			// Read 500 bytes ahead of the pointer to the return value
	    	var data = Memory.readByteArray(retval, 500)
	    	console.log("Data dump (onLeave): ");
	    	console.log(data);
			console.log("outside decrypt");
	    }
	});
}

setTimeout(intercept, 0);
```
In addition, the function `wait_for_lib_load()` is introduced. This is because the library isn't loaded as soon as the app starts, which is a pain because we're hooking to the app when it starts. Therefore, we need a "wait" function, to allow the library to be loaded.

If you try without the wait, you'll get the error `can't add to a base of null` or something along those lines.

Now, when running the python script:

```bash
$ python spawn.py   
[*] Attaching to process
[*] Starting Interceptor
inside decrypt function
Data dump (onLeave):
           0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF
00000000  21 00 00 00 17 00 00 00 00 d9 bb bb 10 68 9e df  !............h..
00000010  08 74 65 73 74 00 00 00 00 00 00 00 5c d3 ce e7  .test.......\...
00000020  19 16 16 fa 01 00 00 00 10 38 cd b7 58 ce 03 c3  .........8..X...
00000030  9c 36 cd b7 ce 21 01 c3 a0 fe 6e ed 54 36 cd b7  .6...!....n.T6..
00000040  50 36 cd b7 e0 54 7c c3 10 38 cd b7 e0 54 7c c3  P6...T|..8...T|.
00000050  10 01 e4 12 c1 d0 49 c3 a0 fe 6e ed 4c 36 cd b7  ......I...n.L6..
00000060  50 36 cd b7 54 36 cd b7 01 00 00 00 00 00 00 00  P6..T6..........
00000070  00 00 00 00 00 00 00 00 90 a1 d6 de 00 00 00 00  ................
00000080  03 00 00 00 e0 c6 40 13 88 01 e4 12 10 01 e4 12  ......@.........
00000090  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
000000a0  00 00 00 00 00 00 00 00 9c 36 cd b7 e0 54 7c c3  .........6...T|.
000000b0  10 38 cd b7 23 29 66 e7 00 00 00 00 e0 c6 40 13  .8..#)f.......@.
000000c0  88 01 e4 12 10 01 e4 12 01 00 00 00 00 00 00 00  ................
000000d0  00 00 00 00 dd 54 7c c3 10 68 9e df 5c d3 ce e7  .....T|..h..\...
000000e0  38 37 cd b7 82 73 6f e7 90 a1 d6 de 08 38 cd b7  87...so......8..
000000f0  10 00 00 00 10 68 9e df d8 3c cd b7 dd 54 7c c3  .....h...<...T|.
00000100  00 00 00 00 aa 72 6f e7 00 00 00 00 00 00 00 00  .....ro.........
00000110  00 00 00 00 bc 68 9e df 00 00 00 00 00 00 00 00  .....h..........
00000120  00 00 00 00 00 00 00 00 00 00 00 00 a0 09 48 ed  ..............H.
00000130  00 00 00 00 5d e4 74 e7 40 6b 96 70 00 00 00 00  ....].t.@k.p....
00000140  00 00 00 00 08 3e cd b7 d0 39 cd b7 a0 09 48 ed  .....>...9....H.
00000150  19 16 16 fa c8 37 cd b7 a0 fe 6e ed 5c d3 ce e7  .....7....n.\...
00000160  98 37 cd b7 10 00 00 00 10 68 9e df 5c d3 ce e7  .7.......h..\...
00000170  b8 37 cd b7 02 d7 8a e7 90 a1 d6 de 10 68 9e df  .7...........h..
00000180  08 38 cd b7 10 00 00 00 d8 3c cd b7 dd 54 7c c3  .8.......<...T|.
00000190  28 38 cd b7 9d d5 8a e7 b0 fe 6e ed 5c d3 ce e7  (8........n.\...
000001a0  90 a1 d6 de 00 00 00 00 e8 80 9c 70 10 1c 78 ed  ...........p..x.
000001b0  19 16 16 fa 5c d3 ce e7 18 4f 42 13 59 9c 5c e7  ....\....OB.Y.\.
000001c0  07 00 00 00 10 68 9e df a0 fe 6e ed 5c d3 ce e7  .....h....n.\...
000001d0  19 16 16 fa ea 09 cb e7 c8 37 cd b7 10 68 9e df  .........7...h..
000001e0  19 00 00 00 30 38 cd b7 10 68 9e df 5c d3 ce e7  ....08...h..\...
000001f0  c8 38 cd b7                                      .8..
outside decrypt
```

I entered the password as `test`, and my password is there, in the memory. But we're attached to the `aes_cbc_decrypt` function ... so we could probably assume this function also checks the password?

If we look 500 bytes behind the current return value, ie. making this change in the `onLeave` function:
```javascript
var data = Memory.readByteArray(ptr(retval - 500), 500);
```
And now run it:
```bash
$ python spawn.py   
[*] Attaching to process
[*] Starting Interceptor
inside decrypt function
Data dump (onLeave):
           0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF
00000000  00 00 00 00 ff ff 00 00 00 00 00 00 00 00 00 00  ................
00000010  00 00 00 d8 17 40 00 00 00 00 00 00 00 00 00 00  .....@..........
00000020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00000030  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00000040  00 34 5b 9d 1b 40 00 00 00 00 00 00 04 01 04 00  .4[..@..........
00000050  00 00 3c a0 00 00 00 00 00 00 00 00 70 69 6c 6c  ..<.........pill
00000060  32 3a 64 72 69 76 61 62 6c 65 3a 31 ad 5c 31 b0  2:drivable:1.\1.
00000070  04 93 fc 3f e9 be db 5e 48 e3 c9 46 31 a7 66 36  ...?...^H..F1.f6
00000080  57 cd 0f 0d 3b 69 da 3e 6b 81 5e 61 3a 73 70 69  W...;i.>k.^a:spi
00000090  6c 6c 32 3a 64 72 69 76 61 62 6c 65 3a 31 09 09  ll2:drivable:1..
000000a0  09 09 09 09 09 09 09 ad 5c 31 b0 04 93 fc 3f e9  ........\1....?.
000000b0  be db 5e 48 e3 c9 46 31 a7 66 36 57 cd 0f 0d 3b  ..^H..F1.f6W...;
000000c0  69 da 3e 6b 81 5e 61 19 16 16 fa 01 3b 69 da 3e  i.>k.^a.....;i.>
000000d0  6b 81 5e 61 3c ff 89 49 6b 32 86 44 50 5b 5c 7a  k.^a<..Ik2.DP[\z
000000e0  3b da 02 1b 69 88 26 ab 02 ba a0 ef 52 e1 fc 95  ;...i.&.....R...
000000f0  69 3b fe 8e 8f 33 3f 52 8d 89 9f bd df 68 63 28  i;...3?R.....hc(
00000100  b6 53 9d a6 6a 6d 1b 1c e7 e4 84 a1 38 8c e7 89  .S..jm......8...
00000110  8e df 7a 2f e4 b7 0e 05 03 53 8a a4 3b df 6d 2d  ..z/.....S..;.m-
00000120  b5 00 17 02 a7 47 79 d0 a4 14 f3 74 9f cb 9e 59  .....Gy....t...Y
00000130  2a cb 89 5b f8 e0 40 35 5c f4 b3 41 c3 3f 2d 18  *..[..@5\..A.?-.
00000140  e9 f4 a4 43 c7 a9 5a 2b 9b 5d e9 6a 58 62 c4 72  ...C..Z+.].jXb.r
00000150  b1 96 60 31 4c 79 9d e3 d7 24 74 89 8f 46 b0 fb  ..`1Ly...$t..F..
00000160  3e d0 d0 ca 0a 09 e9 51 dd 2d 9d d8 52 6b 2d 23  >......Q.-..Rk-#
00000170  6c bb fd e9 6a 9c 4d e5 73 58 c6 7d 9d 92 f0 d9  l...j.M.sX.}....
00000180  81 66 4f a5 73 74 6f 76 65 3a 73 70 69 6c 6c 32  .fO.stove:spill2
00000190  3a 64 72 69 76 61 62 6c 65 3a 31 09 09 09 09 09  :drivable:1.....
000001a0  09 09 09 09 ad 5c 31 b0 04 93 fc 3f e9 be db 5e  .....\1....?...^
000001b0  48 e3 c9 46 d0 05 05 c3 10 18 ec b7 01 00 00 00  H..F............
000001c0  9c 16 ec b7 b0 15 ec b7 58 be 07 c3 0d 00 00 00  ........X.......
000001d0  50 00 00 00 c8 15 ec b7 86 02 00 00 59 10 05 c3  P...........Y...
000001e0  50 ca 80 bb 00 00 00 00 77 7d 5c e7 00 00 43 00  P.......w}\...C.
000001f0  00 00 00 00                                      ....
outside decrypt
```

The closest string to our input is `stove:spill2:drivable:1` which could definitely be a password. It meets the definition of a complex password - symbols, numbers, lowercase ascii (note that we don't collect the `.fO.` in the password, as the `.` (which is actually `0x4f` can't be represented in an ascii format, so therefore wouldn't be part of the password)).

If we enter that into the login form, it says wrong username or password. Of course, we also need a username. So let's do the same for the username. There's the checkUsername function, which inside that, is a call to the function `encryptDecryptUsername`. That offset is `0x8280`.

`hook_native.js` now looks like:
```javascript
function wait_for_lib_load() {
	var remaining = 2;
	if (--remaining > 0) {
		setTimeout(intercept, 5000);
	}
}

function intercept() {

	wait_for_lib_load();
	console.log('[*] Starting Interceptor for password');
	Interceptor.attach(Module.findBaseAddress('libnative-lib.so').add(0x85d0), {
	    onEnter: function (args) {
		console.log("inside decrypt function");
	    },
	    onLeave: function(retval) {
	    	var pwd = Memory.readCString(ptr(retval - 0x70)); // offset of password is "retval - 0x70"
	    	console.log("password = " + pwd.substring(0, 23));
		console.log("outside decrypt");
	    }
	});

	console.log('[*] Starting Interceptor for username');
	Interceptor.attach(Module.findBaseAddress('libnative-lib.so').add(0x8280), {
	    onEnter: function (args) {
				console.log("inside checkUsername function");
	    },
	    onLeave: function(retval) {
	    	var data = Memory.readByteArray(retval, 500);
	    	console.log("data dump (onLeave - username): ");
	    	console.log(data);
			console.log("outside checkUsername");
	    }
	});
}

setTimeout(intercept, 0);
```

And running that:
```bash
$ python spawn.py
[*] Attaching to process
[*] Starting Interceptor for password
[*] Starting Interceptor for username
inside checkUsername function
data dump (onLeave - username):
           0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF
00000000  0e 31 32 6a 61 6e 65 54 00 00 00 00 10 30 9e df  .12janeT.....0..
00000010  0e 53 51 5d 58 5f 55 6c 00 00 00 00 00 00 00 00  .SQ]X_Ul........
00000020  19 16 16 fa 00 00 00 00 10 d8 bb b7 58 7e 7b c3  ............X~{.
00000030  9c d6 bb b7 c0 d1 78 c3 30 fb 6e ed 10 01 dc 12  ......x.0.n.....
00000040  50 d6 bb b7 26 14 66 e7 10 30 9e df e0 64 50 bd  P...&.f..0...dP.
00000050  10 01 dc 12 c1 e0 1d bd 30 fb 6e ed 4c d6 bb b7  ........0.n.L...
00000060  50 d6 bb b7 54 d6 bb b7 00 00 00 00 00 00 00 00  P...T...........
00000070  00 00 00 00 00 00 00 00 90 a1 d6 de 00 00 00 00  ................
00000080  03 00 00 00 58 bc 48 13 88 01 dc 12 10 01 dc 12  ....X.H.........
00000090  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
000000a0  00 00 00 00 00 00 00 00 9c d6 bb b7 e0 64 50 bd  .............dP.
000000b0  10 d8 bb b7 23 29 66 e7 00 00 00 00 58 bc 48 13  ....#)f.....X.H.
000000c0  88 01 dc 12 10 01 dc 12 00 00 00 00 00 00 00 00  ................
000000d0  00 00 00 00 dd 64 50 bd 10 30 9e df 5c d3 ce e7  .....dP..0..\...
000000e0  38 d7 bb b7 82 73 6f e7 90 a1 d6 de 08 d8 bb b7  8....so.........
000000f0  10 00 00 00 10 30 9e df d8 dc bb b7 dd 64 50 bd  .....0.......dP.
00000100  00 00 00 00 aa 72 6f e7 00 00 00 00 00 00 00 00  .....ro.........
00000110  00 00 00 00 bc 30 9e df 00 00 00 00 00 00 00 00  .....0..........
00000120  00 00 00 00 00 00 00 00 00 00 00 00 a0 09 48 ed  ..............H.
00000130  00 00 00 00 5d e4 74 e7 40 6b 96 70 00 00 00 00  ....].t.@k.p....
00000140  00 00 00 00 08 de bb b7 d0 d9 bb b7 a0 09 48 ed  ..............H.
00000150  19 16 16 fa c8 d7 bb b7 30 fb 6e ed 5c d3 ce e7  ........0.n.\...
00000160  98 d7 bb b7 10 00 00 00 10 30 9e df 5c d3 ce e7  .........0..\...
00000170  b8 d7 bb b7 02 d7 8a e7 90 a1 d6 de 10 30 9e df  .............0..
00000180  08 d8 bb b7 10 00 00 00 d8 dc bb b7 dd 64 50 bd  .............dP.
00000190  28 d8 bb b7 9d d5 8a e7 40 fb 6e ed 5c d3 ce e7  (.......@.n.\...
000001a0  90 a1 d6 de 00 00 00 00 e8 80 9c 70 10 1c 78 ed  ...........p..x.
000001b0  19 16 16 fa 5c d3 ce e7 38 8d 4a 13 59 9c 5c e7  ....\...8.J.Y.\.
000001c0  07 00 00 00 10 30 9e df 30 fb 6e ed 5c d3 ce e7  .....0..0.n.\...
000001d0  19 16 16 fa ea 09 cb e7 c8 d7 bb b7 10 30 9e df  .............0..
000001e0  19 00 00 00 30 d8 bb b7 10 30 9e df 5c d3 ce e7  ....0....0..\...
000001f0  c8 d8 bb b7                                      ....
outside checkUsername
inside decrypt function
password = stove:spill2:drivable:1
outside decrypt
```
The first string is `12jammyT`. So now, just to clean the hooking script a bit:

### Flagging
[Jump back to the top](#writeup)

```javascript
function wait_for_lib_load() {
	var remaining = 2;
	if (--remaining > 0) {
		setTimeout(intercept, 5000);
	}
}

function intercept() {

	wait_for_lib_load();
	console.log('[*] Starting Interceptor');
	Interceptor.attach(Module.findBaseAddress('libnative-lib.so').add(0x85d0), {
	    onLeave: function(retval) {
	    	var pwd = Memory.readCString(ptr(retval - 0x70));
	    	console.log('\t[+] Password = ' + pwd.substring(0, 23));
	    }
	});

	Interceptor.attach(Module.findBaseAddress('libnative-lib.so').add(0x8280), {
	    onLeave: function(retval) {
	    	var username = Memory.readCString(retval);
	    	console.log('\t[+] Username = ' + username);
	    }
	});
}

setTimeout(intercept, 0);
```

With the python script:
```python
import frida, sys, time

app_name = 'com.example.jammys_old_infra'
device = frida.get_usb_device()
pid = device.spawn(app_name)
device.resume(pid)
time.sleep(1)

print('[*] Attaching to process')
process = device.attach(pid)
jscode = open('hook_native.js').read()
script = process.create_script(jscode)
script.load()
sys.stdin.read()
```

And running:
```bash
$ python spawn.py
[*] Attaching to process
[*] Starting Interceptor
	[+] Username = 12jammyT
	[+] Password = stove:spill2:drivable:1
[*] Starting Interceptor
```

So now, if we enter in those credentials, the toast notification pops up, and we get our flag.

(
We can use `adb` to enter in the password with the following command:
`$ adb -s emulator-5554 shell input text 'stove:spill2:drivable:1'`
Making sure that the password box is the box to enter text in.
)

![flag = "rarctf{pl34s3_d0nt_s0lv3_st4tic4lly_3829103890}"](./images/flagged.png)

```c++
rarctf{pl34s3_d0nt_s0lv3_st4tic4lly_3829103890}
```
