package com.chymeravr.adclient;

import com.android.volley.RequestQueue;
import com.android.volley.Response;

import lombok.Getter;
import lombok.RequiredArgsConstructor;
import lombok.Setter;

/**
 * Created by robin_chimera on 12/6/2016.
 */

@RequiredArgsConstructor(suppressConstructorProperties = true)
abstract class ServerListener<T> implements Response.ErrorListener, Response.Listener<T> {
    @Getter
    protected final Ad ad;

    @Getter
    @Setter
    private RequestQueue requestQueue;

}

