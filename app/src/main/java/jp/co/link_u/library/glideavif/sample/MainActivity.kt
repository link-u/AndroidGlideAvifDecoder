package jp.co.link_u.library.glideavif.sample

import android.content.ContentResolver
import android.graphics.drawable.Drawable
import android.net.Uri
import android.os.Bundle
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import com.bumptech.glide.Glide
import com.bumptech.glide.RequestBuilder
import com.bumptech.glide.load.resource.drawable.DrawableTransitionOptions.withCrossFade

class MainActivity : AppCompatActivity() {
    private lateinit var imageViewRes1: ImageView
    private lateinit var imageViewRes2: ImageView
    private lateinit var imageViewRes3: ImageView
    private lateinit var requestBuilder: RequestBuilder<Drawable>

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        imageViewRes1 = findViewById(R.id.svg_image_view1)
        imageViewRes2 = findViewById(R.id.svg_image_view2)
        imageViewRes3 = findViewById(R.id.svg_image_view3)

        requestBuilder = Glide.with(this)
            .asDrawable()
            .transition(withCrossFade())
    }

    override fun onStart() {
        super.onStart()
        val uri = Uri.parse(
            ContentResolver.SCHEME_ANDROID_RESOURCE
                    + "://"
                    + packageName
                    + "/"
                    + R.raw.avif_sample
        )

        requestBuilder.load(uri).into(imageViewRes1)

        val uri2 = Uri.parse(
            ContentResolver.SCHEME_ANDROID_RESOURCE
                    + "://"
                    + packageName
                    + "/"
                    + R.raw.avif_sample2
        )

        requestBuilder.load(uri2).into(imageViewRes2)

        val uri3 = Uri.parse(
            ContentResolver.SCHEME_ANDROID_RESOURCE
                    + "://"
                    + packageName
                    + "/"
                    + R.raw.avif_sample3
        )

        requestBuilder.load(uri3).into(imageViewRes3)
    }
}
