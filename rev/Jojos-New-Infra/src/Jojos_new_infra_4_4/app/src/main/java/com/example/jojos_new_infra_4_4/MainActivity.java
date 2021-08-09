package com.example.jojos_new_infra_4_4;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ViewSwitcher;

import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private static final String WEBSITE = "http://ec2-18-130-135-40.eu-west-2.compute.amazonaws.com:9090";
    //private boolean DEBUG = true;

    // User overrides this func to just print out something lmao ....
    // Could call throughout different parts of the program?
    // activity.override(Ljava/lang/String).implementation = function() { console.log("Bypassed all checks"); }
    // Just going to chuck this in onCreate() and .... user can override all the functions .... apart from frida, that needs to be bypassed first
    /*
    public int anti_tech(checkStuff checkStuff) {
    */
    /*
     * [x] Tampering
     * [-] CRC check?
     * [x] Root
     * [-] Native check for rooting files - su, superuser, xposed, magisk, etc.
     * [] Frida / Drozer / Xposed
     * [x] check /proc/self/maps for "frida-agent-64.so" and "libfrida-gadget.so"
     * [-] D-Bus AUTH - send a D-Bus AUTH msg to every open port, check for answer
     * This crashes the app
     * [] Native check TCP port 27042 through /proc/net/tcp
     * [] Might as well check for adb(d) on port 5039
     * [x] Proguard
     * [x] Debuggable
     * [-] Timer check - see whether a process is being debugged by the amount of time spent running
     * [x] Native check of tracerPid != 0 (therefore debugging)
     * [] GOT tampering
     */
    /*
        if (DEBUG) {
            //if (checkStuff.dBusAuthMsgRec()) {
            //    Log.d("DEBUGGING", "Closing down");
            //}
            return 0;
        } else {
            if (checkStuff.checkRoot() ||
                    !checkStuff.checkAppSignature(this) ||
                    checkStuff.isDebuggable(this) ||
                    checkStuff.anyDebuggers() ||
                    !checkStuff.checkMaps()) {
                Log.d("Jojo's New Infra", "Sheesh, trying to meddle with my app? Shame on you.");
                finish();
                System.exit(1); // User could hook System.exit() and stop it executing.
            }
        }
        return 0;
    }
    */

    @SuppressLint("SetJavaScriptEnabled")
    public void main(checkStuff checkStuff, ViewSwitcher vSwitch) {
        WebView webView = findViewById(R.id.webview);
        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webView.setWebViewClient(new WebViewClient() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView webView1, final WebResourceRequest request) {

                new Thread(() -> {
                    String query = null;
                    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                        query = String.valueOf(request.getUrl().getEncodedQuery());
                    }
                    List<String> params = Arrays.asList(query.split("="));
                    if (params.size() > 1 && !(query.equals("v=4.7.0"))) {
                        String pass = null;
                        pass = params.get(params.size() - 1).split("&")[0];
                        String username = null;
                        username = params.get(1).split("&")[0];
                        checkStuff.checkCredentialsUserProvides(vSwitch, username, pass);
                    }
                }).start();
                return super.shouldInterceptRequest(webView1, request);
            }
        });
        webView.loadUrl(WEBSITE);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.viewswitcher);

        ViewSwitcher vSwitch = findViewById(R.id.viewswitcher1);
        checkStuff checkStuff = new checkStuff(this, vSwitch);
        if (checkStuff.checkRoot() ||
                !checkStuff.checkAppSigs(this) ||
                !checkStuff.isDebuggable(this) ||
                !checkStuff.checkMaps() ||
                checkStuff.anyDebuggers()) {
            Log.d("Jojo's New Infra 4.4", "Sheesh, trying to meddle with my app? Shame on you.");
            finish();
            System.exit(1); // User could hook System.exit() and stop it executing.
        }
        main(checkStuff, vSwitch);
    }
}
