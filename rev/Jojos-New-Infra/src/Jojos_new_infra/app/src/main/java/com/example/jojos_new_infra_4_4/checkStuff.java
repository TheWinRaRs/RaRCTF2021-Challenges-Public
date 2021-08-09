package com.example.jojos_new_infra_4_4;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Handler;
import android.os.Looper;
import android.util.Base64;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;
import android.widget.ViewSwitcher;

import com.scottyab.rootbeer.BuildConfig;
import com.scottyab.rootbeer.RootBeer;
import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.FileReader;
import java.security.MessageDigest;
import java.util.HashSet;

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

    public boolean checkAppSignature(Context context) {
        try {
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(
                    context.getPackageName(),
                    PackageManager.GET_SIGNING_CERTIFICATES);
            for (Signature signature : packageInfo.signingInfo.getSigningCertificateHistory()) {
                byte[] signatureBytes = signature.toByteArray();
                MessageDigest md = MessageDigest.getInstance("SHA1");
                md.update(signatureBytes);
                final String currentSig = Base64.encodeToString(md.digest(),
                        Base64.DEFAULT).trim();

                if (cS(currentSig, BuildConfig.DEBUG)) {
                    return true;
                }
            }

        } catch (Exception ignored) { }
        return false;
    }

    public void checkCredentialsUserProvides(ViewSwitcher vSwitch, String username, String pass) {
        if (cC(username, pass, 0) & cC(username, pass, 1)) {
            new Handler(Looper.getMainLooper()).post(() -> {
                Toast toast = Toast.makeText(activity, "Well done, you've got the flag!", Toast.LENGTH_SHORT);
                toast.show();
            });
        } else {
            new Handler(Looper.getMainLooper()).post(() -> {
                Button button1 = activity.findViewById(R.id.button_orig);
                button1.setOnClickListener(v -> vSwitch.showNext());
                Log.d("DEBUGGING", "It's not a bug, it's a feature!");
            });
        }
    }


    public native boolean cC(String username, String password, int lookingFor); // renamed from checkCreds()
    public native boolean cS(String javaSig, boolean isInDebugMode); // renamed from checkSignature()
    public native boolean ctp(); // renamed from checkTracerPid()

}
