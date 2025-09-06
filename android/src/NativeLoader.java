package com.raylib.catfishers;

//import android.app.Activity;
//import android.os.Bundle;
//import android.view.View;
//public class NativeLoader extends android.app.NativeActivity {
//    static {
//        System.loadLibrary("main");
//        System.loadLibrary("raymoblib");
//        System.loadLibrary("raylib");
//    }
//
//    @Override
//    protected void onResume() {
//        super.onResume();
//    }
//
//    private native void startClient();
//}


import android.app.NativeActivity;
import android.view.KeyEvent;
import android.os.Bundle;

public class NativeLoader extends NativeActivity {

    public DisplayManager displayManager;
    public SoftKeyboard softKeyboard;
    public boolean initCallback = false;

    // Loading method of your native application
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        displayManager = new DisplayManager(this);
        softKeyboard = new SoftKeyboard(this);
        System.loadLibrary("main");
        System.loadLibrary("raymoblib");
        System.loadLibrary("raylib");
    }

    // Handling loss and regain of application focus
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (false && hasFocus) {
            displayManager.setImmersiveMode(); // If the app has focus, re-enable immersive mode
        }
    }

    // Callback methods for managing the Android software keyboard
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        softKeyboard.onKeyUpEvent(event);
        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onStart() {
        super.onStart();
        if(initCallback) {
            onAppStart();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(initCallback) {
            onAppResume();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if(initCallback) {
            onAppPause();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        if(initCallback){
            onAppStop();
        }
    }

    private native void onAppStart();
    private native void onAppResume();
    private native void onAppPause();
    private native void onAppStop();

}