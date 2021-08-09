package com.example.jojos_new_infra_4_4;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Base64;
import android.util.Log;
import android.widget.Toast;
import android.widget.ViewSwitcher;

import com.scottyab.rootbeer.RootBeer;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.FileReader;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;

import static android.os.Debug.isDebuggerConnected;

public class checkStuff extends AppCompatActivity {

    static {
        System.loadLibrary("native-lib");
    }

    public Activity activity;
    public ViewSwitcher vSwitch;

    public checkStuff(Activity _activity, ViewSwitcher vSwitch) {
        this.activity = _activity;
        this.vSwitch = vSwitch;
    }

    public boolean checkRoot() {
        RootBeer rootBeer = new RootBeer(activity);
        return rootBeer.isRooted();
    }

    public boolean isDebuggable(Context context) {
        boolean result = (context.getApplicationContext().getApplicationInfo().flags & ApplicationInfo.FLAG_DEBUGGABLE) != 0;
        return !(result & isDebuggerConnected());
    }

    public boolean anyDebuggers() {
        return !ctp();
    }

    public boolean checkMaps() {
        try {
            HashSet<String> libraries = new HashSet<>();
            String mapsFilename = "/proc/" + android.os.Process.myPid() + "/maps";
            BufferedReader reader = new BufferedReader(new FileReader(mapsFilename));
            String line;
            while ((line = reader.readLine()) != null) {
                if (line.endsWith(".so") || line.endsWith(".jar")) {
                    int n = line.lastIndexOf(" ");
                    libraries.add(line.substring(n + 1));
                }
            }
            for (String library : libraries) {
                if (library.contains("com.saurik.substrate") ||
                        library.contains("XposedBridge.jar") ||
                        library.contains("re.frida.server") ||
                        library.contains("libfrida-gadget.so") ||
                        library.contains("drozer")) {
                    return false;
                }
            }
        } catch (Exception ignored) {  }
        return true;
    }


    private static byte[] signatureDigest(Signature sig) {
        byte[] signature = sig.toByteArray();
        try {
            MessageDigest md = MessageDigest.getInstance("SHA1");
            byte[] digest = md.digest(signature);
            return Base64.encode(digest, Base64.DEFAULT);
        } catch (NoSuchAlgorithmException e) {
            return null;
        }
    }
    private static List<String> signatureDigest(Signature[] sigList) {
        List<String> signaturesList= new ArrayList<>();
        for (Signature signature: sigList) {
            if(signature!=null) {
                signaturesList.add(Arrays.toString(signatureDigest(signature)));
            }
        }
        return signaturesList;
    }

    private static List<String> checkAppSignature(@NonNull PackageManager pm, @NonNull String packageName) {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                PackageInfo packageInfo = pm.getPackageInfo(packageName, PackageManager.GET_SIGNING_CERTIFICATES);
                if (packageInfo == null
                        || packageInfo.signingInfo == null) {
                    return null;
                }
                if(packageInfo.signingInfo.hasMultipleSigners()){
                    return signatureDigest(packageInfo.signingInfo.getApkContentsSigners());
                }
                else{
                    return signatureDigest(packageInfo.signingInfo.getSigningCertificateHistory());
                }
            }
            else {
                @SuppressLint("PackageManagerGetSignatures")
                PackageInfo packageInfo = pm.getPackageInfo(packageName, PackageManager.GET_SIGNATURES);
                if (packageInfo == null
                        || packageInfo.signatures == null
                        || packageInfo.signatures.length == 0
                        || packageInfo.signatures[0] == null) {
                    return null;
                }
                return signatureDigest(packageInfo.signatures);
            }
        } catch (PackageManager.NameNotFoundException e) {
            return null;
        }
    }

    public boolean checkAppSigs(Context context) {
        List<String> sigs = checkAppSignature(context.getPackageManager(), context.getPackageName());

        for (String sig : sigs) {
            String[] sigArray = sig.replace("[", "").replace("]", "").replace(" ", "").split(",");
            String[] sigAscii = new String[sigArray.length];
            for (int i = 0; i < sigArray.length; i++) {
                sigAscii[i] = String.valueOf((char)Integer.parseInt(sigArray[i]));
            }
            String newSig = TextUtils.join("",sigAscii).replace("\n", "");
            //Log.d("DEBUGGING", "Signatures = " + newSig);
            if (cS(newSig, 0)) {
                //Log.d("DEBUGGING", "Signature match");
                return true;
            }

        }
        return false;
    }

    public void checkCredentialsUserProvides(ViewSwitcher vSwitch, String username, String pass) {
        if (cC(username, pass, 0) & cC(username, pass, 1)) {
            new Handler(Looper.getMainLooper()).post(() -> {
                Toast toast = Toast.makeText(activity, "Well done, you've got the flag!", Toast.LENGTH_SHORT);
                toast.show();
            });
        } /*else {
            new Handler(Looper.getMainLooper()).post(() -> {
                Button button1 = activity.findViewById(R.id.button_orig);
                button1.setOnClickListener(v -> vSwitch.showNext());
                Log.d("DEBUGGING", "It's not a bug, it's a feature!");
            });
        }*/
    }


    public native boolean cC(String username, String password, int lookingFor); // renamed from checkCreds()
    public native boolean cS(String javaSig, int isInDebugMode); // renamed from checkSignature()
    public native boolean ctp(); // renamed from checkTracerPid()

}
