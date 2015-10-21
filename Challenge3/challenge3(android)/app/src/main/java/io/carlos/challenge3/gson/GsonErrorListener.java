package io.carlos.challenge3.gson;

import android.content.Context;
import android.app.AlertDialog;
import android.util.Log;

import com.android.volley.NoConnectionError;
import com.android.volley.ParseError;
import com.android.volley.Response;
import com.android.volley.TimeoutError;
import com.android.volley.VolleyError;
import com.google.gson.Gson;
import com.google.gson.JsonSyntaxException;

import io.carlos.challenge3.models.Message;

/**
 * Created by carlos on 8/13/2015.
 */
public class GsonErrorListener implements Response.ErrorListener {
    private Context context;

    public GsonErrorListener(Context c){
        this.context = c;
    }

    @Override
    public void onErrorResponse(VolleyError error) {
        if(error instanceof TimeoutError || error instanceof NoConnectionError)
        {
            AlertDialog.Builder builder = new AlertDialog.Builder(context);
            builder.setMessage("Please check your network connection.").setPositiveButton("Okay",null).show();
            Log.e("dbg", error.toString());
        } else if(error instanceof ParseError){
            AlertDialog.Builder builder = new AlertDialog.Builder(context);
            builder.setMessage("Whoops. Looks like you're running an outdated version of this app.").setPositiveButton("Okay",null).show();
        } else {
            Gson gson = new Gson();
            try{
                String responseBody = new String(error.networkResponse.data);
                Message msg = gson.fromJson(responseBody, Message.class);
                AlertDialog.Builder builder = new AlertDialog.Builder(context);
                builder.setMessage(msg.msg).setPositiveButton("Okay",null).show();
            } catch (JsonSyntaxException e){
                AlertDialog.Builder builder = new AlertDialog.Builder(context);
                builder.setMessage("Hmmm... Something went horribly wrong here.").setPositiveButton("Okay", null).show();
            }
        }
    }
}
