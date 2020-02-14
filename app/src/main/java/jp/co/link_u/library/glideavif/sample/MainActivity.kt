package jp.co.link_u.library.glideavif.sample

import android.graphics.drawable.Drawable
import android.os.Bundle
import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.bumptech.glide.Glide
import com.bumptech.glide.RequestBuilder
import com.bumptech.glide.load.resource.drawable.DrawableTransitionOptions.withCrossFade

class MainActivity : AppCompatActivity() {
    private lateinit var recyclerView: RecyclerView
    private lateinit var viewAdapter: RecyclerView.Adapter<*>
    private lateinit var viewManager: RecyclerView.LayoutManager

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val requestBuilder = Glide.with(this)
            .asDrawable()
            .transition(withCrossFade())

        viewManager = LinearLayoutManager(this)
        viewAdapter = MyAdapter(requestBuilder, images)

        recyclerView = findViewById<RecyclerView>(R.id.image_list).apply {
            setHasFixedSize(true)
            layoutManager = viewManager
            adapter = viewAdapter
        }
    }

    class MyAdapter(
        private val requestBuilder: RequestBuilder<Drawable>,
        private val images: Array<String>
    ) :
        RecyclerView.Adapter<MyAdapter.MyViewHolder>() {

        class MyViewHolder(v: LinearLayout) : RecyclerView.ViewHolder(v) {
            val textView: TextView = v.findViewById(R.id.textView)
            val imageView: ImageView = v.findViewById(R.id.imageView)
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyViewHolder {
            val v = LayoutInflater.from(parent.context)
                .inflate(R.layout.image_cell, parent, false) as LinearLayout
            return MyViewHolder(v)
        }

        override fun onBindViewHolder(holder: MyViewHolder, position: Int) {
            val name = images[position]

            holder.textView.text = name.split(".").joinToString("\n")
            requestBuilder
                .load("https://github.com/link-u/avif-sample-images/raw/master/$name")
                .into(holder.imageView)
        }

        override fun getItemCount() = images.size - 1
    }
}

val images =
    arrayOf(
        "fox.profile0.8bpc.yuv420.avif",
        "fox.profile0.8bpc.yuv420.odd-width.avif",
        "fox.profile0.8bpc.yuv420.odd-height.avif",
        "fox.profile0.8bpc.yuv420.odd-width.odd-height.avif",
        "fox.profile0.8bpc.yuv420.monochrome.avif",
        "fox.profile0.8bpc.yuv420.monochrome.odd-width.avif",
        "fox.profile0.8bpc.yuv420.monochrome.odd-height.avif",
        "fox.profile0.8bpc.yuv420.monochrome.odd-width.odd-height.avif",
        "fox.profile0.10bpc.yuv420.avif",
        "fox.profile0.10bpc.yuv420.odd-width.avif",
        "fox.profile0.10bpc.yuv420.odd-height.avif",
        "fox.profile0.10bpc.yuv420.odd-width.odd-height.avif",
        "fox.profile0.10bpc.yuv420.monochrome.avif",
        "fox.profile0.10bpc.yuv420.monochrome.odd-width.avif",
        "fox.profile0.10bpc.yuv420.monochrome.odd-height.avif",
        "fox.profile0.10bpc.yuv420.monochrome.odd-width.odd-height.avif",
        "fox.profile2.12bpc.yuv420.avif",
        "fox.profile2.12bpc.yuv420.odd-width.avif",
        "fox.profile2.12bpc.yuv420.odd-height.avif",
        "fox.profile2.12bpc.yuv420.odd-width.odd-height.avif",
        "fox.profile2.12bpc.yuv420.monochrome.avif",
        "fox.profile2.12bpc.yuv420.monochrome.odd-width.avif",
        "fox.profile2.12bpc.yuv420.monochrome.odd-height.avif",
        "fox.profile2.12bpc.yuv420.monochrome.odd-width.odd-height.avif",
        "fox.profile2.8bpc.yuv422.avif",
        "fox.profile2.8bpc.yuv422.odd-width.avif",
        "fox.profile2.8bpc.yuv422.odd-height.avif",
        "fox.profile2.8bpc.yuv422.odd-width.odd-height.avif",
        "fox.profile2.8bpc.yuv422.monochrome.avif",
        "fox.profile2.8bpc.yuv422.monochrome.odd-width.avif",
        "fox.profile2.8bpc.yuv422.monochrome.odd-height.avif",
        "fox.profile2.8bpc.yuv422.monochrome.odd-width.odd-height.avif",
        "fox.profile2.10bpc.yuv422.avif",
        "fox.profile2.10bpc.yuv422.odd-width.avif",
        "fox.profile2.10bpc.yuv422.odd-height.avif",
        "fox.profile2.10bpc.yuv422.odd-width.odd-height.avif",
        "fox.profile2.10bpc.yuv422.monochrome.avif",
        "fox.profile2.10bpc.yuv422.monochrome.odd-width.avif",
        "fox.profile2.10bpc.yuv422.monochrome.odd-height.avif",
        "fox.profile2.10bpc.yuv422.monochrome.odd-width.odd-height.avif",
        "fox.profile2.12bpc.yuv422.avif",
        "fox.profile2.12bpc.yuv422.odd-width.avif",
        "fox.profile2.12bpc.yuv422.odd-height.avif",
        "fox.profile2.12bpc.yuv422.odd-width.odd-height.avif",
        "fox.profile2.12bpc.yuv422.monochrome.avif",
        "fox.profile2.12bpc.yuv422.monochrome.odd-width.avif",
        "fox.profile2.12bpc.yuv422.monochrome.odd-height.avif",
        "fox.profile2.12bpc.yuv422.monochrome.odd-width.odd-height.avif",
        "fox.profile1.8bpc.yuv444.avif",
        "fox.profile1.8bpc.yuv444.odd-width.avif",
        "fox.profile1.8bpc.yuv444.odd-height.avif",
        "fox.profile1.8bpc.yuv444.odd-width.odd-height.avif",
        "fox.profile1.10bpc.yuv444.avif",
        "fox.profile1.10bpc.yuv444.odd-width.avif",
        "fox.profile1.10bpc.yuv444.odd-height.avif",
        "fox.profile1.10bpc.yuv444.odd-width.odd-height.avif",
        "fox.profile2.12bpc.yuv444.avif",
        "fox.profile2.12bpc.yuv444.odd-width.avif",
        "fox.profile2.12bpc.yuv444.odd-height.avif",
        "fox.profile2.12bpc.yuv444.odd-width.odd-height.avif",
        "fox.profile2.12bpc.yuv444.monochrome.avif",
        "fox.profile2.12bpc.yuv444.monochrome.odd-width.avif",
        "fox.profile2.12bpc.yuv444.monochrome.odd-height.avif",
        "fox.profile2.12bpc.yuv444.monochrome.odd-width.odd-height.avif",
        ""
    )
