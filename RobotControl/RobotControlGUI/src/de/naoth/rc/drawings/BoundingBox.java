package de.naoth.rc.drawings;

import java.awt.AlphaComposite;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Composite;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GraphicsConfiguration;
import java.awt.Image;
import java.awt.Paint;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.Shape;
import java.awt.Stroke;
import java.awt.font.FontRenderContext;
import java.awt.font.GlyphVector;
import java.awt.geom.AffineTransform;
import java.awt.geom.Rectangle2D;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.image.BufferedImageOp;
import java.awt.image.ImageObserver;
import java.awt.image.RenderedImage;
import java.awt.image.renderable.RenderableImage;
import java.text.AttributedCharacterIterator;
import java.util.Map;

/**
 * Generates a bounding box of the applied drawings.
 * If something is drawn on this "graphics", a bounding box is calculated,
 * which encloses the complete drawing.
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class BoundingBox extends Graphics2D {

    private Rectangle2D.Double boundingBox = new Rectangle2D.Double(0, 0, 0, 0);

    private AffineTransform transform = new AffineTransform();
    private Font font = new Font(Font.DIALOG, Font.PLAIN, 12);
    private Color background;
    private Color foreground;
    private Stroke stroke = new BasicStroke();
    private Paint paint;
    private Composite composite = AlphaComposite.SrcOver;
    private RenderingHints hints = new RenderingHints(null);

    private void update(double x, double y, double width, double height) {
        if (x < boundingBox.x) {
            boundingBox.x = x;
        }
        if (y < boundingBox.y) {
            boundingBox.y = y;
        }
        if (x + width > boundingBox.x + boundingBox.width) {
            boundingBox.width += (x + width - (boundingBox.x + boundingBox.width));
        }
        if (y + height > boundingBox.y + boundingBox.height) {
            boundingBox.height += (y + height - (boundingBox.y + boundingBox.height));
        }
    }
    
    @Override
    public String toString() {
        return String.format("left: %f; right: %f; width: %f; height: %f", boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height);
    }
    
    public double getX() {
        return boundingBox.x;
    }

    public double getY() {
        return boundingBox.y;
    }

    public double getWidth() {
        return boundingBox.width;
    }

    public double getHeight() {
        return boundingBox.height;
    }

    public double getX2() {
        return boundingBox.x+boundingBox.width;
    }

    public double getY2() {
        return boundingBox.y+boundingBox.height;
    }
    
    public Rectangle2D getBoundingBox() {
        return boundingBox;
    }
    
    @Override
    public void draw(Shape s) {
        this.update(s.getBounds().x, s.getBounds().y, s.getBounds().width, s.getBounds().height);
    }
    
    public void drawImage(BufferedImage img, BufferedImageOp op, double x, double y) {
        BufferedImage out = op.filter(img, null);
        this.update(x, y, out.getWidth(), out.getHeight());
    }

    @Override
    public boolean drawImage(Image img, AffineTransform xform, ImageObserver obs) {
        // INFO: don't know if that's a correct implementation!?
        drawImage(new BufferedImage(img.getWidth(null), img.getHeight(null), 0), 
                  new AffineTransformOp(xform, null), 
                  xform.getTranslateX(), 
                  xform.getTranslateY());
        return true;
    }

    @Override
    public void drawImage(BufferedImage img, BufferedImageOp op, int x, int y) {
        this.drawImage(img, op, x, y);
    }

    @Override
    public void drawRenderedImage(RenderedImage img, AffineTransform xform) {
        // INFO: don't know if that's a correct implementation!?
        this.drawImage(new BufferedImage(img.getWidth(), img.getHeight(), 0),
                       new AffineTransformOp(xform, null),
                       xform.getTranslateX(),
                       xform.getTranslateY());
    }

    @Override
    public void drawRenderableImage(RenderableImage img, AffineTransform xform) {
        // INFO: don't know if that's a correct implementation!?
        this.drawImage(new BufferedImage((int)img.getWidth(), (int)img.getHeight(), 0),
                       new AffineTransformOp(xform, null),
                       xform.getTranslateX(),
                       xform.getTranslateY());
    }

    @Override
    public void drawString(String str, int x, int y) {
        this.update(x, y, 0, 0);
        // TODO: calc width/height of the string to get accurate dimensions
    }

    @Override
    public void drawString(String str, float x, float y) {
        this.update(x, y, 0, 0);
        // TODO: calc width/height of the string to get accurate dimensions
    }

    @Override
    public void drawString(AttributedCharacterIterator iterator, int x, int y) {
        this.update(x, y, 0, 0);
        // TODO: can we do sométhing with the AttributedCharacterIterator???
    }

    @Override
    public void drawString(AttributedCharacterIterator iterator, float x, float y) {
        this.update(x, y, 0, 0);
        // TODO: can we do sométhing with the AttributedCharacterIterator???
    }

    @Override
    public void drawGlyphVector(GlyphVector g, float x, float y) {
        this.update(x, y, 0, 0);
        // TODO: can we do sométhing with the GlyphVector???
    }

    @Override
    public void fill(Shape s) {
        this.update(s.getBounds().x, s.getBounds().y, s.getBounds().width, s.getBounds().height);
    }

    @Override
    public boolean hit(Rectangle rect, Shape s, boolean onStroke) {
        // INFO: hopefully this isn't a much needed functionality ?!
        return true;
    }

    @Override
    public GraphicsConfiguration getDeviceConfiguration() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void setComposite(Composite comp) {
        if(comp != null) {
            this.composite = comp;
        }
    }

    @Override
    public void setPaint(Paint paint) {
        if(paint != null) {
            this.paint = paint;
        }
    }

    @Override
    public void setStroke(Stroke s) {
        if(s != null) {
            this.stroke = s;
        }
    }

    @Override
    public void setRenderingHint(RenderingHints.Key hintKey, Object hintValue) {
        this.hints.put(hintKey, hintValue);
    }

    @Override
    public Object getRenderingHint(RenderingHints.Key hintKey) {
        return this.hints.get(hintKey);
    }

    @Override
    public void setRenderingHints(Map<?, ?> hints) {
        this.hints = new RenderingHints((Map<RenderingHints.Key, ?>) hints);
    }

    @Override
    public void addRenderingHints(Map<?, ?> hints) {
        this.hints.add((RenderingHints) hints);
    }

    @Override
    public RenderingHints getRenderingHints() {
        return this.hints;
    }

    @Override
    public void translate(int x, int y) {
        boundingBox = (Rectangle2D.Double) AffineTransform.getTranslateInstance(x, y).createTransformedShape(boundingBox).getBounds2D();
        this.transform.translate(x, y);
    }

    @Override
    public void translate(double tx, double ty) {
        boundingBox = (Rectangle2D.Double) AffineTransform.getTranslateInstance(tx, ty).createTransformedShape(boundingBox).getBounds2D();
        this.transform.translate(tx, ty);
    }

    @Override
    public void rotate(double theta) {
        boundingBox = (Rectangle2D.Double) AffineTransform.getRotateInstance(theta).createTransformedShape(boundingBox).getBounds2D();
        this.transform.rotate(theta);
    }

    @Override
    public void rotate(double theta, double x, double y) {
        AffineTransform af = new AffineTransform();
        af.rotate(theta, x, y);
        boundingBox = (Rectangle2D.Double) af.createTransformedShape(boundingBox).getBounds2D();
        this.transform.rotate(theta, x, y);
    }

    @Override
    public void scale(double sx, double sy) {
        // INFO: if we scale something, we should also scale all drawings!
//        boundingBox = (Rectangle2D.Double) AffineTransform.getScaleInstance(sx, sy).createTransformedShape(boundingBox).getBounds2D();
        this.transform.scale(sx, sy);
    }

    @Override
    public void shear(double shx, double shy) {
//        boundingBox = (Rectangle2D.Double) AffineTransform.getShearInstance(shx, shy).createTransformedShape(boundingBox).getBounds2D();
        this.transform.shear(shx, shy);
    }

    @Override
    public void transform(AffineTransform Tx) {
        // INFO: the transform could include some scaling (see scale()) - currently ignoring
//        boundingBox = (Rectangle2D.Double) Tx.createTransformedShape(boundingBox).getBounds2D();
        this.transform.concatenate(Tx);
    }

    @Override
    public void setTransform(AffineTransform Tx) {
        // INFO: the transform could include some scaling (see scale()) - currently ignoring
//        boundingBox = (Rectangle2D.Double) Tx.createTransformedShape(boundingBox).getBounds2D();
        this.transform = Tx;
    }

    @Override
    public AffineTransform getTransform() {
        return this.transform;
    }

    @Override
    public Paint getPaint() {
        if(this.paint == null) {
            return foreground;
        }
        return paint;
    }

    @Override
    public Composite getComposite() {
        return this.composite;
    }

    @Override
    public void setBackground(Color color) {
        this.background = color;
    }

    @Override
    public Color getBackground() {
        return this.background;
    }

    @Override
    public Stroke getStroke() {
        return this.stroke;
    }

    @Override
    public void clip(Shape s) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public FontRenderContext getFontRenderContext() {
        return new FontRenderContext(this.transform, true, true);
    }

    @Override
    public Graphics create() {
        try {
            return (Graphics)clone();
        } catch (CloneNotSupportedException ex) {
        }
        return null;
    }

    @Override
    public Color getColor() {
        return this.foreground;
    }

    @Override
    public void setColor(Color c) {
        this.foreground = c;
    }

    @Override
    public void setPaintMode() {
        setComposite(AlphaComposite.SrcOver);
    }

    @Override
    public void setXORMode(Color c1) {
        if(c1 != null) {
            // TODO: setXORMode
//            setComposite(new XORComposite(c1, sd));
        }
    }

    @Override
    public Font getFont() {
        return this.font;
    }

    @Override
    public void setFont(Font font) {
        if(font != null) {
            this.font = font;
        }
    }

    @Override
    public FontMetrics getFontMetrics(Font f) {
//        return FontDesignMetrics.getMetrics(font);
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Rectangle getClipBounds() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void clipRect(int x, int y, int width, int height) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void setClip(int x, int y, int width, int height) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Shape getClip() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void setClip(Shape clip) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void copyArea(int x, int y, int width, int height, int dx, int dy) { /* ignoring */ }

    @Override
    public void drawLine(int x1, int y1, int x2, int y2) {
        this.update(Math.min(x1, x2), Math.min(y1, y2), Math.abs(x1 - x2), Math.abs(y1 - y2));
    }

    @Override
    public void fillRect(int x, int y, int width, int height) {
        this.update(x, y, width, height);
    }

    @Override
    public void clearRect(int x, int y, int width, int height) {
        this.update(x, y, width, height);
    }

    @Override
    public void drawRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) {
        this.update(x, y, width, height);
    }

    @Override
    public void fillRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) {
        this.update(x, y, width, height);
    }

    @Override
    public void drawOval(int x, int y, int width, int height) {
        this.update(x, y, width, height);
    }

    @Override
    public void fillOval(int x, int y, int width, int height) {
        this.update(x, y, width, height);
    }

    @Override
    public void drawArc(int x, int y, int width, int height, int startAngle, int arcAngle) {
        // TODO: drawArc
    }

    @Override
    public void fillArc(int x, int y, int width, int height, int startAngle, int arcAngle) {
        // TODO: fillArc
    }

    @Override
    public void drawPolyline(int[] xPoints, int[] yPoints, int nPoints) {
        // TODO: drawPolyline
    }

    @Override
    public void drawPolygon(int[] xPoints, int[] yPoints, int nPoints) {
        // TODO: drawPolygon
    }

    @Override
    public void fillPolygon(int[] xPoints, int[] yPoints, int nPoints) {
        // TODO: fillPolygon
    }

    @Override
    public boolean drawImage(Image img, int x, int y, ImageObserver observer) {
        if(img != null) {
            this.update(x, y, img.getWidth(observer), img.getHeight(observer));
        }
        return true;
    }

    @Override
    public boolean drawImage(Image img, int x, int y, int width, int height, ImageObserver observer) {
        if(img != null) {
            this.update(x, y, width, height);
        }
        return true;
    }

    @Override
    public boolean drawImage(Image img, int x, int y, Color bgcolor, ImageObserver observer) {
        if(img != null) {
            this.update(x, y, img.getWidth(observer), img.getHeight(observer));
        }
        return true;
    }

    @Override
    public boolean drawImage(Image img, int x, int y, int width, int height, Color bgcolor, ImageObserver observer) {
        if(img != null) {
            this.update(x, y, width, height);
        }
        return true;
    }

    @Override
    public boolean drawImage(Image img, int dx1, int dy1, int dx2, int dy2, int sx1, int sy1, int sx2, int sy2, ImageObserver observer) {
        if(img != null && dx1 != dx2 && dy1 != dy2) {
            this.update(Math.min(dx1, dx2), Math.min(dy1, dy2), Math.max(dx1, dx2)-Math.min(dx1,dx2), Math.max(dy1, dy2)-Math.min(dy1,dy2));
        }
        return true;
    }

    @Override
    public boolean drawImage(Image img, int dx1, int dy1, int dx2, int dy2, int sx1, int sy1, int sx2, int sy2, Color bgcolor, ImageObserver observer) {
        if(img != null && dx1 != dx2 && dy1 != dy2) {
            this.update(Math.min(dx1, dx2), Math.min(dy1, dy2), Math.max(dx1, dx2)-Math.min(dx1,dx2), Math.max(dy1, dy2)-Math.min(dy1,dy2));
        }
        return true;
    }

    @Override
    public void dispose() { /* ignoring */ }
}
