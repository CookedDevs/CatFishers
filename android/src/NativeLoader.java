package com.raylib.catfishers;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
public class NativeLoader extends android.app.NativeActivity {
    static {
        System.loadLibrary("main");   // ensure your app library loaded first
        System.loadLibrary("raylib");
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    private native void startClient();
}