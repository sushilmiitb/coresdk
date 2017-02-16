package com.chymeravr.adclient;

import android.content.Context;

import com.chymeravr.analytics.AnalyticsManager;
import com.chymeravr.common.WebRequestQueue;
import com.chymeravr.schemas.serving.AdFormat;

import org.json.JSONObject;

/**
 * Created by robin_chimera on 12/6/2016.
 */

/* A fake ad class implementation for testing. this doesnt do anything*/
public class FakeAd extends Ad {
    public FakeAd(Context context, AdListener adListener, AnalyticsManager analyticsManager,
                  WebRequestQueue webRequestQueue) {
        super(AdFormat.IMG_360, "test", context, adListener, analyticsManager, webRequestQueue);
    }

    @Override
    public void loadAd(AdRequest adRequest) {
        this.setLoading(true);
    }

    @Override
    public void show() {

    }

    @Override
    void onAdServerResponseSuccess(JSONObject response) {

    }

    @Override
    void onMediaServerResponseSuccess() {

    }


}
