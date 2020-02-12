package jp.co.link_u.library.glideavif

import android.graphics.drawable.BitmapDrawable
import com.bumptech.glide.load.Options
import com.bumptech.glide.load.engine.Resource
import com.bumptech.glide.load.resource.SimpleResource
import com.bumptech.glide.load.resource.transcode.ResourceTranscoder

class AvifDrawableTranscoder : ResourceTranscoder<Avif, BitmapDrawable> {
    override fun transcode(
        toTranscode: Resource<Avif>,
        options: Options
    ): Resource<BitmapDrawable> {
        val avif = toTranscode.get()
        val drawable = BitmapDrawable(null, avif.bitmap)
        return SimpleResource(drawable)
    }
}
