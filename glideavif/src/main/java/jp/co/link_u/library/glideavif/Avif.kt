package jp.co.link_u.library.glideavif

import android.graphics.Bitmap
import android.util.Log
import java.io.ByteArrayOutputStream
import java.io.InputStream

class Avif(inputStream: InputStream) {
    var bitmap: Bitmap
        private set

    init {
        val byteArray = toByteArray(inputStream)

        val colors = decodeAvif2(byteArray, byteArray.size)
        val width = colors[0]
        val height = colors[1]
        this.bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        this.bitmap.setPixels(
            colors,
            2,
            width,
            0, 0,
            width,
            height
        )
//        this.bitmap =
//            Bitmap.createBitmap(
//                colors,
//                2,
//                width,
//                width,
//                height,
//                Bitmap.Config.ARGB_8888
//            )
//
//        this.bitmap.setColorSpace(ColorSpace.get(ColorSpace.Named.ADOBE_RGB))
    }

    private external fun decodeAvif(
        bitmap: Bitmap,
        byteArray: ByteArray,
        byteArrayLength: Int
    ): Int

    private external fun decodeAvif2(
        byteArray: ByteArray,
        byteArrayLength: Int
    ): IntArray

    companion object {
        fun getFromInputStream(s: InputStream): Avif {
            Log.d("hogehoge", "hogehoge")
            return Avif(s)
        }

        private fun toByteArray(s: InputStream): ByteArray {
            val buffer = ByteArrayOutputStream()

            var nRead: Int
            val data = ByteArray(16384)

            while (true) {
                nRead = s.read(data, 0, data.size)
                if (nRead == -1) {
                    break
                }

                buffer.write(data, 0, nRead)
            }
            return buffer.toByteArray()
        }

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("dav1d")
            System.loadLibrary("avif")
            System.loadLibrary("native-lib")
        }
    }
}
