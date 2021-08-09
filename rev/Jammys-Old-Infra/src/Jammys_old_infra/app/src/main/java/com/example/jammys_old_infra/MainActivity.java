package com.example.jammys_old_infra;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Toast;
import android.widget.ViewSwitcher;

import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private static final String WEBSITE = "http://ec2-18-130-135-40.eu-west-2.compute.amazonaws.com:8090";

    @SuppressLint("SetJavaScriptEnabled")
    private void main() {
        final ViewSwitcher viewSwitcher = (ViewSwitcher) findViewById(R.id.viewswitcher);
        WebView webView = (WebView) findViewById(R.id.webview);
        webView.getSettings().setJavaScriptEnabled(true);
        webView.setWebViewClient(new WebViewClient() {

            @Override
            public WebResourceResponse shouldInterceptRequest(
                    WebView webView,
                    final WebResourceRequest webResourceRequest) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        String query = String.valueOf(webResourceRequest.getUrl().getEncodedQuery());
                        List params = Arrays.asList(query.split("="));
                        if (params.size() > 1 && !query.equals("v=4.7.0")) {
                            String pass = null;
                            try {
                                pass = URLDecoder.decode(((String) params.get(params.size() - 1)).split("&")[0], StandardCharsets.UTF_8.toString());
                            } catch (UnsupportedEncodingException e) {
                                e.printStackTrace();
                            }
                            String username = null;
                            try {
                                username = URLDecoder.decode(((String) params.get(1)).split("&")[0], StandardCharsets.UTF_8.toString());
                            } catch (UnsupportedEncodingException e2) {
                                e2.printStackTrace();
                            }
                            checkCredentialsUserProvides(viewSwitcher, username, pass);
                        }
                    }
                }).start();
                return super.shouldInterceptRequest(webView, webResourceRequest);
            }
        });
        webView.loadUrl(WEBSITE);
    }

    private void checkCredentialsUserProvides(ViewSwitcher vSwitch, String username, String pass) {
        if (checkCreds(username, pass, 1) && checkCreds(username, pass, 0)) {
            new Handler(Looper.getMainLooper()).post(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(MainActivity.this.getApplicationContext(), "Well done, you've got the flag!", Toast.LENGTH_SHORT).show();
                }
            });
        } else {
            new Handler(Looper.getMainLooper()).post((Runnable) () -> {
                vSwitch.showNext();
                MainActivity.this.findViewById(R.id.button_orig).setOnClickListener(
                        view -> vSwitch.showNext());
            });
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.viewswitcher);
        main();
    }

    public native boolean checkCreds(String username, String password, int lookingFor);

}
