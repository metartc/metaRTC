//
// Copyright (c) 2019-2022 yanggaofeng
//
package com.metartc.push;

import android.Manifest;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;


public class YangMainActivity extends AppCompatActivity implements View.OnClickListener {
    private Button m_b_play=null;
    private EditText m_url=null;
    private SurfaceView m_surface_view=null;
    private boolean m_startingPlay=false;
    private boolean isPermissionGranted = false;
    private YangPush m_push=null;
    public final static int Yang_RC_CAMERA = 100;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        if(m_push==null) m_push=new YangPush(YangPush.Camera_Front,YangPush.Encoder_cpu,YangPush.MediaServer_srs);
        init();
        initSurfaceView();
        // response screen rotation event
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_FULL_SENSOR);

        requestPermission();
    }
    private void init() {
        m_b_play = findViewById(R.id.m_b_player);
        m_url=findViewById(R.id.m_t_url);
        m_b_play.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        if(m_startingPlay){
            m_b_play.setText("start");
            m_startingPlay=false;
            m_push.stopPush();
        }else {
            if (m_push.startPush( m_url.getText().toString()) == 0) {
                m_startingPlay = true;
                m_b_play.setText("stop");
            }
        }
    }

    private void initSurfaceView() {
        m_surface_view = findViewById(R.id.m_v_surfaceView);
        m_surface_view.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {

                m_push.setSurface(holder.getSurface(),640,480,30);
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                m_push.setSurfaceSize(width, height);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                m_push.releaseResources();
            }
        });
    }

    private void requestPermission() {
        //1. 检查是否已经有该权限
        if (Build.VERSION.SDK_INT >= 23 && (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED || ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO)
                != PackageManager.PERMISSION_GRANTED || ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED)) {
            //2. 权限没有开启，请求权限
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO, Manifest.permission.WRITE_EXTERNAL_STORAGE}, Yang_RC_CAMERA);
        }else{
            //权限已经开启，做相应事情
            isPermissionGranted = true;
            init();
        }
    }
}