//
// Copyright (c) 2019-2022 yanggaofeng
//
package com.metartc.player;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;



public class YangMainActivity extends YangBaseActivity implements View.OnClickListener {
    private Button m_b_play=null;
    private EditText m_url=null;
    private YangYuvPlayer m_surface_view=null;
    private boolean m_startingPlay=false;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        init();
        initSurfaceView();
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
            YangAndroidPlay.getInstance().stopPlayer();
        }else {
            if (YangAndroidPlay.getInstance().startPlayer( m_url.getText().toString()) == 0) {
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
                YangAndroidPlay.getInstance().setSurface(holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                YangAndroidPlay.getInstance().setSurfaceSize(width, height);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                YangAndroidPlay.getInstance().releaseResources();
            }
        });
    }
}