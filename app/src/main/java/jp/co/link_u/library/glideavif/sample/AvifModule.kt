package jp.co.link_u.library.glideavif.sample

import android.content.Context
import android.graphics.Bitmap
import com.bumptech.glide.Glide
import com.bumptech.glide.Registry
import com.bumptech.glide.annotation.GlideModule
import com.bumptech.glide.integration.okhttp3.OkHttpUrlLoader
import com.bumptech.glide.load.model.GlideUrl
import com.bumptech.glide.module.AppGlideModule
import jp.co.link_u.library.glideavif.AvifDecoderFromByteBuffer
import java.io.InputStream
import java.nio.ByteBuffer

@GlideModule
class AvifModule : AppGlideModule() {
    override fun registerComponents(context: Context, glide: Glide, registry: Registry) {
        registry
//            .append(
//                InputStream::class.java,
//                Bitmap::class.java,
//                AvifDecoder()
//            )
            .prepend(
                ByteBuffer::class.java,
                Bitmap::class.java,
                AvifDecoderFromByteBuffer()
            )
            .replace(
                GlideUrl::class.java,
                InputStream::class.java,
                OkHttpUrlLoader.Factory(UnsafeOkHttpClient.getUnsafeOkHttpClient())
            )
    }

    // Disable manifest parsing to avoid adding similar modules twice.
    override fun isManifestParsingEnabled(): Boolean {
        return false
    }
}
