package com.raylib.catfishers;

import android.view.inputmethod.InputMethodManager;
import android.app.NativeActivity;
import android.content.Context;
import android.view.KeyEvent;
import android.util.Log;
import android.view.View;

public class SoftKeyboard {

    private static final String TAG = "SoftKeyboard";

    private final Context context;
    private final InputMethodManager imm;
    private KeyEvent lastKeyEvent = null;

    public SoftKeyboard(Context context) {
        imm = (InputMethodManager)context.getSystemService(Context.INPUT_METHOD_SERVICE);
        this.context = context;
        Log.d(TAG, "SoftKeyboard created, imm=" + (imm != null));
    }

    /* PUBLIC FOR JNI (raymob.h) */

    public void showKeyboard() {
        Log.d(TAG, "showKeyboard() called");
        imm.showSoftInput(((NativeActivity)context).getWindow().getDecorView(), InputMethodManager.SHOW_FORCED);
    }

    public void hideKeyboard() {
        imm.hideSoftInputFromWindow(((NativeActivity)context).getWindow().getDecorView().getWindowToken(), 0);
    }

    public int getLastKeyCode() {
        if (lastKeyEvent != null) return lastKeyEvent.getKeyCode();
        return 0;
    }

    public char getLastKeyLabel() {
        if (lastKeyEvent != null) return lastKeyEvent.getDisplayLabel();
        return '\0';
    }

    public int getLastKeyUnicode() {
        if (lastKeyEvent != null) return lastKeyEvent.getUnicodeChar();
        return 0;
    }

    public void clearLastKeyEvent() {
        lastKeyEvent = null;
    }

    /* PRIVATE FOR JNI (raymob.h) */

    public void onKeyUpEvent(KeyEvent event) {
        lastKeyEvent = event;
    }
}