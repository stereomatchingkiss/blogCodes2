package adv;

import com.appbucks.sdk.AppBucksAPI;

import org.qtproject.qt5.android.bindings.QtApplication;
import org.qtproject.qt5.android.bindings.QtActivity;

//import android.util.Log;
import android.os.Bundle;
//import android.os.IBinder;
//import android.content.ServiceConnection;
//import android.content.Intent;
//import android.content.ComponentName;
//import android.content.Context;
//import android.app.PendingIntent;
//import org.json.JSONObject;

public class appBucksAdv extends QtActivity
{
    private static appBucksAdv m_instance;

    public appBucksAdv()
    {
        m_instance = this;
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
      super.onCreate(savedInstanceState);

      AppBucksAPI.initialize(this, "chartUnitConverter",
      1, "4700d885-5382-4749-8f1e-06e0351f1b6a", false, null, null);
    }

    //public static void notify(String s)
    public static String printHelloWorld(int a)
    {
      return "hello world";
    }
}
