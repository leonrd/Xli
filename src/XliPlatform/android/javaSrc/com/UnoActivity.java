package $(Android.PackageName);
$(UnoCore.UnoActivity.Top.Statement:Join())

import android.app.Activity;
import android.app.NativeActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.LinearLayout;
import $(Android.PackageName).UnoSurfaceView;

public class $(Android.LibraryName) extends android.app.NativeActivity
{
    static 
    {
        $(Android.Java.LoadLibraries)
    }
        
    public static NativeActivity RootActivity;
    public static LinearLayout RootLayout;
    public static UnoSurfaceView RootView;
    public static EditText REMOVE_THIS; // {TODO} this is here as a test, remove asap
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {    	
    	Log.d("XliApp", "Caching Native Activity");
    	RootActivity = this;
    	com.Shim.XliJ.CacheActivity(this);
    	Log.d("XliApp", "Successfully Cached Native Activity");
    	
        super.onCreate(savedInstanceState);
        
        
        
        RootView = new UnoSurfaceView(RootActivity);
        LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        RootView.setLayoutParams(layoutParams);
        //RootView.setVisibility(View.INVISIBLE);
        
        REMOVE_THIS = new EditText(RootActivity);
        LinearLayout.LayoutParams layoutParams2 = new LinearLayout.LayoutParams(300, 300);
        REMOVE_THIS.setLayoutParams(layoutParams2);
        REMOVE_THIS.setVisibility(View.VISIBLE);

        RootLayout = new LinearLayout(RootActivity);
        RootLayout.setBackgroundColor(Color.GREEN);
        RootLayout.addView(REMOVE_THIS);
        RootLayout.addView(RootView);
        
        //setContentView(RootLayout);
        getWindow().takeSurface(null);
        getWindow().setContentView(RootLayout);
    }

    //------------------------------------------------
    
    $(UnoCore.UnoActivity.Body.Statement:Join())
}
