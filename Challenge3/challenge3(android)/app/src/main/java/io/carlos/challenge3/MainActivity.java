package io.carlos.challenge3;

import android.app.Activity;
import android.app.AlertDialog;
import android.graphics.Color;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.toolbox.Volley;

import io.carlos.challenge3.gson.GsonErrorListener;
import io.carlos.challenge3.gson.GsonGet;
import io.carlos.challenge3.models.Message;

public class MainActivity extends Activity {
    Button on, off, status;
    LinearLayout container;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        container = (LinearLayout) findViewById(R.id.container);
        on = new Button(this);
        off = new Button(this);
        status = new Button(this);

        on.setText("Turn On");
        on.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                RequestQueue queue = Volley.newRequestQueue(MainActivity.this);
                GsonGet<Message> turnOn = new GsonGet<Message>("/turnOnAll", Message.class, 5000, new Response.Listener<Message>() {
                    @Override
                    public void onResponse(Message response) {
                        container.setBackgroundColor(Color.parseColor("#EFC600"));
                        String msg = response.msg.replace(";", "\n");
                        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                        builder.setMessage(msg).setPositiveButton("Okay", null).show();
                    }
                }, new GsonErrorListener(MainActivity.this));

                queue.add(turnOn);
            }
        });

        off.setText("Turn Off");
        off.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                RequestQueue queue = Volley.newRequestQueue(MainActivity.this);
                GsonGet<Message> turnOn = new GsonGet<Message>("/turnOffAll", Message.class, 5000, new Response.Listener<Message>() {
                    @Override
                    public void onResponse(Message response) {
                        container.setBackgroundColor(getResources().getColor(android.R.color.transparent));
                        String msg = response.msg.replace(";", "\n");
                        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                        builder.setMessage(msg).setPositiveButton("Okay", null).show();
                    }
                }, new GsonErrorListener(MainActivity.this));

                queue.add(turnOn);
            }
        });

        status.setText("Get Status");
        status.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                RequestQueue queue = Volley.newRequestQueue(MainActivity.this);
                GsonGet<Message> turnOn = new GsonGet<Message>("/getStatus", Message.class, 5000, new Response.Listener<Message>() {
                    @Override
                    public void onResponse(Message response) {
                        String msg = response.msg.replace(";", "\n");
                        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                        builder.setMessage(msg).setPositiveButton("Okay", null).show();
                    }
                }, new GsonErrorListener(MainActivity.this));

                queue.add(turnOn);
            }
        });

        container.addView(on);
        container.addView(off);
        container.addView(status);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
