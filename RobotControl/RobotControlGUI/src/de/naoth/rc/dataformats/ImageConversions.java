/*
 * 
 */
package de.naoth.rc.dataformats;

import com.google.protobuf.ByteString;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;

/**
 *
 * @author Heinrich Mellmann
 */
public class ImageConversions {

    public static class DimensionMismatchException extends RuntimeException {

        public DimensionMismatchException() {
        }

        public DimensionMismatchException(String s) {
            super(s);
        }
    }
    
    public static void convertYUV422toYUV888(ByteString bs, BufferedImage image) {
        // we should have exactly 2 bytes per pixel
        if (image.getWidth() * image.getHeight() * 2 != bs.size()) {
            throw new DimensionMismatchException();
        }

        int[] rgbBuffer = ((DataBufferInt) image.getRaster().getDataBuffer()).getData();
        for (int i = 0; i < rgbBuffer.length / 2; i++) {
            // convert to 'unsigned'
            int y1 = 0xFF & bs.byteAt(i * 4);
            int u = 0xFF & bs.byteAt(i * 4 + 1);
            int y2 = 0xFF & bs.byteAt(i * 4 + 2);
            int v = 0xFF & bs.byteAt(i * 4 + 3);

            rgbBuffer[i * 2] = ((y1 << 16) | (u << 8) | v);
            rgbBuffer[i * 2 + 1] = ((y2 << 16) | (u << 8) | v);
        }
    }//end convertYUV422toYUV888

    public static void convertYUV888toYUV888(ByteString bs, BufferedImage image) {
        // we should have exactly 3 bytes per pixel
        if (image.getWidth() * image.getHeight() * 3 != bs.size()) {
            throw new DimensionMismatchException();
        }

        int[] rgbBuffer = ((DataBufferInt) image.getRaster().getDataBuffer()).getData();
        for (int i = 0; i < rgbBuffer.length; i++) {
            // convert to 'unsigned'
            int y = 0xFF & bs.byteAt(i * 3);
            int u = 0xFF & bs.byteAt(i * 3 + 1);
            int v = 0xFF & bs.byteAt(i * 3 + 2);

            rgbBuffer[i] = ((y << 16) | (u << 8) | v);
        }
    }//end convertYUV888toYUV888

    public static void convertYUV422toRGB888Fast(ByteString bs, BufferedImage image) {
        // we should have exactly 2 bytes per pixel
        if (image.getWidth() * image.getHeight() * 2 != bs.size()) {
            throw new DimensionMismatchException();
        }

        int[] rgbBuffer = ((DataBufferInt) image.getRaster().getDataBuffer()).getData();
        for (int i = 0; i < rgbBuffer.length / 2; i++) {
            // convert to 'unsigned'
            int y1 = 0xFF & bs.byteAt(i * 4);
            int u = 0xFF & bs.byteAt(i * 4 + 1);
            int y2 = 0xFF & bs.byteAt(i * 4 + 2);
            int v = 0xFF & bs.byteAt(i * 4 + 3);

            u = (u - 128);
            v = (v - 128);

            // wikipedia: Integer operation of ITU-R standard for YCbCr(8 bits per channel) to RGB888
            int r1c = y1 + v + (v >> 2) + (v >> 3) + (v >> 5);
            int g1c = y1 - ((u >> 4) + (u >> 5)) - ((v >> 1) + (v >> 2) + (v >> 3) + (v >> 4) + (v >> 5));
            int b1c = y1 + u + (u >> 1) + (u >> 2) + (u >> 6);

            // clamp the calues to [0,255]
            int r1 = (r1c < 0) ? 0 : (r1c > 255) ? 255 : r1c;
            int g1 = (g1c < 0) ? 0 : (g1c > 255) ? 255 : g1c;
            int b1 = (b1c < 0) ? 0 : (b1c > 255) ? 255 : b1c;

            rgbBuffer[i * 2] = ((r1 << 16) | (g1 << 8) | b1);

            // wikipedia: Integer operation of ITU-R standard for YCbCr(8 bits per channel) to RGB888
            int r2c = y2 + v + (v >> 2) + (v >> 3) + (v >> 5);
            int g2c = y2 - ((v >> 2) + (u >> 4) + (u >> 5)) - ((v >> 1) + (v >> 3) + (v >> 4) + (v >> 5));
            int b2c = y1 + u + (u >> 1) + (u >> 2) + (u >> 6);

            // clamp the calues to [0,255]
            int r2 = (r2c < 0) ? 0 : (r2c > 255) ? 255 : r2c;
            int g2 = (g2c < 0) ? 0 : (g2c > 255) ? 255 : g2c;
            int b2 = (b2c < 0) ? 0 : (b2c > 255) ? 255 : b2c;

            rgbBuffer[i * 2 + 1] = ((r2 << 16) | (g2 << 8) | b2);
        }
    }//end convertYUV422toRGB888Fast

    public static void convertYUV888toRGB888Fast(ByteString bs, BufferedImage image) {
        // we should have exactly 3 bytes per pixel
        if (image.getWidth() * image.getHeight() * 3 != bs.size()) {
            throw new DimensionMismatchException();
        }

        int[] rgbBuffer = ((DataBufferInt) image.getRaster().getDataBuffer()).getData();
        for (int i = 0; i < rgbBuffer.length; i++) {
            // convert to 'unsigned'
            int y = 0xFF & bs.byteAt(i * 3);
            int u = 0xFF & bs.byteAt(i * 3 + 1);
            int v = 0xFF & bs.byteAt(i * 3 + 2);

            u = u - 128;
            v = v - 128;

            // wikipedia: Integer operation of ITU-R standard for YCbCr(8 bits per channel) to RGB888
            int rc = y + v + (v >> 2) + (v >> 3) + (v >> 5);
            int gc = y - ((u >> 4) + (u >> 5)) - ((v >> 1) + (v >> 2) + (v >> 3) + (v >> 4) + (v >> 5));
            int bc = y + u + (u >> 1) + (u >> 2) + (u >> 6);

            // clamp the calues to [0,255]
            int r = (rc < 0) ? 0 : (rc > 255) ? 255 : rc;
            int g = (gc < 0) ? 0 : (gc > 255) ? 255 : gc;
            int b = (bc < 0) ? 0 : (bc > 255) ? 255 : bc;

            rgbBuffer[i] = (r << 16) | (g << 8) | b;
        }
    }//end convertYUV888toRGB888Fast

    public static void convertYUV888toRGB888Fast(BufferedImage src, BufferedImage dst) {
        // we should have exactly 3 bytes per pixel
        if (src.getWidth() != dst.getWidth() || src.getHeight() != dst.getHeight()) {
            throw new DimensionMismatchException();
        }

        int[] dataDst = ((DataBufferInt) dst.getRaster().getDataBuffer()).getData();
        int[] dataSrc = ((DataBufferInt) src.getRaster().getDataBuffer()).getData();
        for (int i = 0; i < dataDst.length; i++) {
            int yuv = dataSrc[i];
            int y = 0xFF & (yuv >> 16);
            int u = 0xFF & (yuv >> 8);
            int v = 0xFF &  yuv;

            u = u - 128;
            v = v - 128;

            // wikipedia: Integer operation of ITU-R standard for YCbCr(8 bits per channel) to RGB888
            int rc = y + v + (v >> 2) + (v >> 3) + (v >> 5);
            int gc = y - ((u >> 4) + (u >> 5)) - ((v >> 1) + (v >> 2) + (v >> 3) + (v >> 4) + (v >> 5));
            int bc = y + u + (u >> 1) + (u >> 2) + (u >> 6);

            // clamp the calues to [0,255]
            int r = (rc < 0) ? 0 : (rc > 255) ? 255 : rc;
            int g = (gc < 0) ? 0 : (gc > 255) ? 255 : gc;
            int b = (bc < 0) ? 0 : (bc > 255) ? 255 : bc;

            dataDst[i] = (r << 16) | (g << 8) | b;
        }
    }//end convertYUV888toRGB888Fast
    
    public static void convertYUV888toRGB888Precise(BufferedImage src, BufferedImage dst) {
        // we should have exactly 3 bytes per pixel
        if (src.getWidth() != dst.getWidth() || src.getHeight() != dst.getHeight()) {
            throw new DimensionMismatchException();
        }

        int[] dataDst = ((DataBufferInt) dst.getRaster().getDataBuffer()).getData();
        int[] dataSrc = ((DataBufferInt) src.getRaster().getDataBuffer()).getData();
        for (int i = 0; i < dataDst.length; i++) {
            int yuv = dataSrc[i];
            int y = 0xFF & (yuv >> 16);
            int u = 0xFF & (yuv >> 8);
            int v = 0xFF &  yuv;

            u = u - 128;
            v = v - 128;

            float r1c = y + 1.4021f * v;
            float g1c = y - 0.3456f * u - 0.71448f * v;
            float b1c = y + 1.7710f * u;

            // clamp the calues to [0,255]
            int r = (int)((r1c < 0) ? 0 : (r1c > 255) ? 255 : r1c);
            int g = (int)((g1c < 0) ? 0 : (g1c > 255) ? 255 : g1c);
            int b = (int)((b1c < 0) ? 0 : (b1c > 255) ? 255 : b1c);

            dataDst[i] = (r << 16) | (g << 8) | b;
        }
    }//end convertYUV888toRGB888Precise
    
    public static void convertYUV422toRGB888Precise(ByteString bs, BufferedImage image) {
        // we should have exactly 2 bytes per pixel
        if (image.getWidth() * image.getHeight() * 2 != bs.size()) {
            throw new DimensionMismatchException();
        }

        int[] rgbBuffer = ((DataBufferInt) image.getRaster().getDataBuffer()).getData();
        for (int i = 0; i < rgbBuffer.length / 2; i++) {
            // convert to 'unsigned'
            int y1 = 0xFF & bs.byteAt(i * 4);
            int u  = 0xFF & bs.byteAt(i * 4 + 1);
            int y2 = 0xFF & bs.byteAt(i * 4 + 2);
            int v  = 0xFF & bs.byteAt(i * 4 + 3);

            u = (u - 128);
            v = (v - 128);

            float r1c = y1 + 1.4021f * v;
            float g1c = y1 - 0.3456f * u - 0.71448f * v;
            float b1c = y1 + 1.7710f * u;

            // clamp the calues to [0,255]
            int r1 = (int)((r1c < 0) ? 0 : (r1c > 255) ? 255 : r1c);
            int g1 = (int)((g1c < 0) ? 0 : (g1c > 255) ? 255 : g1c);
            int b1 = (int)((b1c < 0) ? 0 : (b1c > 255) ? 255 : b1c);

            rgbBuffer[i * 2] = ((r1 << 16) | (g1 << 8) | b1);

            float r2c = y2 + 1.4021f * v;
            float g2c = y2 - 0.3456f * u - 0.71448f * v;
            float b2c = y2 + 1.7710f * u;

            // clamp the calues to [0,255]
            int r2 = (int)((r2c < 0) ? 0 : (r2c > 255) ? 255 : r2c);
            int g2 = (int)((g2c < 0) ? 0 : (g2c > 255) ? 255 : g2c);
            int b2 = (int)((b2c < 0) ? 0 : (b2c > 255) ? 255 : b2c);

            rgbBuffer[i * 2 + 1] = ((r2 << 16) | (g2 << 8) | b2);
        }
    }//end convertYUV422toRGB888Fast
    
    
    public static void convertRGB888toYUV888Precise(BufferedImage src, BufferedImage dst) {
        // we should have exactly 3 bytes per pixel
        if (src.getWidth() != dst.getWidth() || src.getHeight() != dst.getHeight()) {
            throw new DimensionMismatchException();
        }

        int[] dataDst = ((DataBufferInt) dst.getRaster().getDataBuffer()).getData();
        int[] dataSrc = ((DataBufferInt) src.getRaster().getDataBuffer()).getData();
        for (int i = 0; i < dataDst.length; i++) {
            int rgb = dataSrc[i];
            int r = 0xFF & (rgb >> 16);
            int g = 0xFF & (rgb >> 8);
            int b = 0xFF &  rgb;
            
            float yc = 0.2990f * r + 0.5870f * g + 0.1140f * b;
            float uc = -0.1687f * r - 0.3313f * g + 0.5000f * b;
            float vc = 0.5000f * r - 0.4187f * g - 0.0813f * b;

            // clamp the calues to [0,255]
            int y = (int)((yc < 0) ? 0 : (yc > 255) ? 255 : yc);
            int u = (int)((uc < 0) ? 0 : (uc > 255) ? 255 : uc);
            int v = (int)((vc < 0) ? 0 : (vc > 255) ? 255 : vc);

            dataDst[i] = ((y << 16) | (u << 8) | v);
        }
    }//end convertYUV888toRGB888Fast

}
