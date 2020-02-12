package jp.co.link_u.library.glideavif

import android.graphics.Bitmap
import android.util.Log
import java.io.ByteArrayOutputStream
import java.io.InputStream

class Avif(inputStream: InputStream) {
    var bitmap: Bitmap
        private set

    init {
        val bitmap = Bitmap.createBitmap(2048, 858, Bitmap.Config.ARGB_8888)
        val byteArray = toByteArray(inputStream)

        decodeAvif(bitmap, byteArray, byteArray.size)
        this.bitmap = bitmap
    }

    private external fun decodeAvif(
        bitmap: Bitmap,
        byteArray: ByteArray,
        byteArrayLength: Int
    ): Int

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
