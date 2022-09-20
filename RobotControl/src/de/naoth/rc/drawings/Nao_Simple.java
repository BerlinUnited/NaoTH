package de.naoth.rc.drawings;

import java.awt.*;
import java.awt.geom.*;


/**
 * This class was originally automatically generated and manually cleaned.
 * <a href="https://flamingo.dev.java.net">Flamingo SVG transcoder</a>.
 */
public class Nao_Simple 
{

  private double headYaw = 0.0;
  private Color color = new Color(50, 62, 234, 255);

  public double getHeadYaw() {
    return this.headYaw;
  }

  public void setHeadYaw(double x) {
    this.headYaw = x;
  }

  public Color getColor() {
    return this.color;
  }

  public void setColor(Color color) {
    this.color = color;
  }

  GeneralPath knee_left;
  GeneralPath knee_right;
  
  GeneralPath head_outer;
  GeneralPath head_inner;
  GeneralPath ear_right;
  GeneralPath ear_left;
  
  GeneralPath shoulder_left;
  GeneralPath shoulder_right;
  
  GeneralPath arm_left;
  GeneralPath arm_right;

  GeneralPath hand_left;
  GeneralPath hand_right;
  
  
  public Nao_Simple() 
  {
        knee_left = new GeneralPath();
        knee_left.moveTo(138.91, 147.14);
        knee_left.curveTo(139.90001, 161.231, 132.90001, 182.691, 125.59901, 187.39);
        knee_left.curveTo(114.049, 184.59, 118.59901, 185.64, 107.04901, 180.741);
        knee_left.lineTo(106.46901, 180.541);
        knee_left.curveTo(107.809006, 176.781, 109.49901, 173.52, 111.19901, 167.281);
        knee_left.curveTo(111.57901, 167.14, 111.93901, 166.981, 112.29901, 166.83101);
        knee_left.curveTo(119.40901, 160.33101, 123.76901, 149.52, 125.48901, 137.671);
        knee_left.lineTo(125.598015, 137.692);
        knee_left.curveTo(133.48, 139.97, 137.85, 143.12, 138.91, 147.14);
        knee_left.closePath(); // left knee

        knee_right = new GeneralPath();
        knee_right.moveTo(125.6, 63.51);
        knee_right.curveTo(132.9, 68.2, 139.9, 89.66, 138.9, 103.76);
        knee_right.curveTo(137.849, 107.78, 133.469, 110.93, 125.59899, 113.21);
        knee_right.lineTo(125.41899, 113.24);
        knee_right.curveTo(123.52899, 100.81, 118.75899, 89.7, 111.24899, 83.81);
        knee_right.curveTo(109.538994, 77.439995, 107.81899, 74.159996, 106.468994, 70.36);
        knee_right.lineTo(107.048996, 70.16);
        knee_right.curveTo(118.6, 65.26, 114.05, 66.31, 125.6, 63.51);
        knee_right.closePath(); // right knee

        head_outer = new GeneralPath();
        head_outer.moveTo(111.2, 167.28);
        head_outer.curveTo(91.179, 175.099, 45.6, 171.20999, 32.46, 168.94);
        head_outer.curveTo(31.359999, 168.75, 30.48, 168.57, 29.869999, 168.41);
        head_outer.curveTo(8.49, 155.23, 9.88, 96.33, 29.29, 81.99);
        head_outer.lineTo(29.35, 82.06);
        head_outer.curveTo(30.140001, 81.85, 31.36, 81.619995, 32.94, 81.369995);
        head_outer.curveTo(47.46, 79.06999, 92.31, 75.6, 111.25, 83.81);
        head_outer.curveTo(118.76, 89.7, 123.53, 100.81, 125.42, 113.24);
        head_outer.curveTo(126.63, 121.18, 126.67, 129.66, 125.49, 137.67);
        head_outer.curveTo(123.769, 149.52, 119.409996, 160.33, 112.299995, 166.83);
        head_outer.curveTo(111.94, 166.98, 111.58, 167.14, 111.2, 167.28);
        head_outer.closePath();
        head_outer.moveTo(119.04, 112.81);
        head_outer.lineTo(119.0, 112.829994);
        head_outer.curveTo(118.96, 112.74, 118.91, 112.63999, 118.86, 112.549995);
        head_outer.curveTo(113.009, 102.119995, 51.879997, 100.159996, 46.85, 109.71);
        head_outer.curveTo(40.019997, 118.46, 41.07, 131.231, 47.199997, 139.87);
        head_outer.curveTo(52.269997, 149.5, 113.869995, 147.56999, 118.769, 136.9);
        head_outer.lineTo(118.829994, 136.929);
        head_outer.curveTo(121.99, 129.91, 122.52, 119.9, 119.04, 112.81);
        head_outer.closePath();
        
        
        head_inner = new GeneralPath();
        head_inner.moveTo(119.04, 112.81);
        head_inner.curveTo(122.519, 119.899994, 121.99, 129.91, 118.83, 136.93);
        head_inner.lineTo(118.769005, 136.90099);
        head_inner.curveTo(113.87, 147.57098, 52.269005, 149.50099, 47.200005, 139.87099);
        head_inner.curveTo(41.070004, 131.23099, 40.020004, 118.46098, 46.850006, 109.71098);
        head_inner.curveTo(51.880005, 100.16098, 113.01001, 102.12099, 118.86001, 112.55098);
        head_inner.curveTo(118.91001, 112.640976, 118.96001, 112.74098, 119.00001, 112.83098);
        head_inner.lineTo(119.04, 112.81);
        head_inner.closePath();
        
        
        ear_right = new GeneralPath();
        ear_right.moveTo(38.04, 67.01);
        ear_right.curveTo(38.21, 66.43, 38.370003, 65.810005, 38.530003, 65.130005);
        ear_right.curveTo(46.870003, 57.350006, 75.020004, 55.060005, 92.54, 59.320004);
        ear_right.curveTo(92.93, 59.420002, 93.31, 59.520004, 93.69, 59.620003);
        ear_right.curveTo(97.679, 60.690002, 101.04, 62.120003, 103.4, 63.930004);
        ear_right.curveTo(103.559, 64.05, 103.71, 64.170006, 103.86, 64.3);
        ear_right.curveTo(104.29, 64.66, 104.679, 65.03001, 105.03, 65.420006);
        ear_right.lineTo(105.049995, 65.420006);
        ear_right.curveTo(105.46, 67.29001, 105.92999, 68.87, 106.46999, 70.36001);
        ear_right.curveTo(107.82099, 74.16001, 109.53999, 77.44001, 111.24999, 83.810005);
        ear_right.curveTo(92.30999, 75.600006, 47.45999, 79.07001, 32.939995, 81.37);
        ear_right.lineTo(32.849995, 80.66);
        ear_right.curveTo(35.03, 73.62, 36.72, 71.64, 38.04, 67.01);
        ear_right.closePath();
        
        ear_left = new GeneralPath();
        ear_left.moveTo(38.04, 183.89);
        ear_left.curveTo(36.63, 179.09, 34.56, 176.43, 32.38, 169.38);
        ear_left.lineTo(32.460003, 168.94101);
        ear_left.curveTo(45.600002, 171.21101, 91.18001, 175.1, 111.2, 167.281);
        ear_left.curveTo(109.5, 173.52, 107.809, 176.781, 106.46999, 180.541);
        ear_left.curveTo(105.92999, 182.031, 105.45999, 183.61101, 105.049995, 185.481);
        ear_left.lineTo(105.03, 185.481);
        ear_left.curveTo(104.679, 185.871, 104.29, 186.241, 103.86, 186.6);
        ear_left.curveTo(103.71, 186.731, 103.559, 186.85, 103.4, 186.97);
        ear_left.curveTo(101.04, 188.77, 97.67, 190.21, 93.679, 191.27);
        ear_left.curveTo(76.28, 195.94101, 47.059002, 193.71, 38.529, 185.76001);
        ear_left.curveTo(38.38, 185.1, 38.21, 184.48, 38.04, 183.89);
        ear_left.closePath();
        
        
        shoulder_right = new GeneralPath();
        shoulder_right.moveTo(35.12, 33.1);
        shoulder_right.lineTo(35.059998, 32.82);
        shoulder_right.curveTo(40.159996, 27.39, 51.17, 23.64, 61.89, 23.86);
        shoulder_right.curveTo(68.52, 24.95, 73.77, 26.44, 78.19, 28.12);
        shoulder_right.curveTo(79.969, 29.18, 81.599, 30.490002, 83.009, 32.030003);
        shoulder_right.curveTo(83.66901, 32.510002, 94.82, 41.050003, 93.769005, 59.310005);
        shoulder_right.lineTo(92.54001, 59.320004);
        shoulder_right.curveTo(75.020004, 55.060005, 46.87001, 57.350002, 38.53001, 65.130005);
        shoulder_right.curveTo(38.37001, 65.810005, 38.21001, 66.43001, 38.04001, 67.01);
        shoulder_right.lineTo(31.100008, 67.36);
        shoulder_right.curveTo(25.29, 57.44, 32.5, 34.16, 35.12, 33.1);
        shoulder_right.closePath();
        
        
        shoulder_left = new GeneralPath();
        shoulder_left.moveTo(35.12, 217.8);
        shoulder_left.curveTo(32.5, 216.74, 25.289999, 193.46, 31.099998, 183.54001);
        shoulder_left.lineTo(38.039997, 183.89001);
        shoulder_left.curveTo(38.209995, 184.48102, 38.379997, 185.10002, 38.53, 185.76001);
        shoulder_left.curveTo(47.059998, 193.71, 76.28, 195.94101, 93.68, 191.27);
        shoulder_left.lineTo(93.77, 191.59001);
        shoulder_left.curveTo(94.821, 209.85, 83.67, 218.38, 83.009995, 218.87001);
        shoulder_left.curveTo(81.59999, 220.41, 79.96999, 221.72002, 78.190994, 222.78001);
        shoulder_left.curveTo(73.770996, 224.46, 68.520996, 225.95001, 61.890995, 227.04001);
        shoulder_left.curveTo(51.170994, 227.26001, 40.160995, 223.51001, 35.060997, 218.08);
        shoulder_left.lineTo(35.12, 217.8);
        shoulder_left.closePath();
        
        
        arm_right = new GeneralPath();
        arm_right.moveTo(90.33, 38.99);
        arm_right.curveTo(98.71, 52.120003, 111.17, 49.050003, 110.990005, 45.59);
        arm_right.curveTo(114.71901, 51.54, 113.51, 59.5, 104.25001, 63.86);
        arm_right.lineTo(103.86001, 64.3);
        arm_right.curveTo(103.71001, 64.170006, 103.559006, 64.05, 103.40001, 63.930004);
        arm_right.curveTo(101.04001, 62.120003, 97.67901, 60.690002, 93.69001, 59.620003);
        arm_right.curveTo(93.30901, 59.520004, 92.92901, 59.420002, 92.54001, 59.320004);
        arm_right.lineTo(93.769005, 59.310005);
        arm_right.curveTo(94.82001, 41.050003, 83.66901, 32.510006, 83.009, 32.030006);
        arm_right.curveTo(81.599, 30.490005, 79.969, 29.180006, 78.19, 28.120007);
        arm_right.curveTo(81.639, 29.420006, 84.589005, 30.830006, 87.299, 32.240005);
        arm_right.lineTo(90.379005, 38.960007);
        arm_right.lineTo(90.33, 38.99);
        arm_right.closePath();

        arm_left = new GeneralPath();
        arm_left.moveTo(90.33, 211.91);
        arm_left.lineTo(90.380005, 211.94);
        arm_left.lineTo(87.3, 218.66);
        arm_left.curveTo(84.590004, 220.07, 81.64, 221.48001, 78.191, 222.78);
        arm_left.curveTo(79.97, 221.719, 81.6, 220.41, 83.01, 218.87);
        arm_left.curveTo(83.670006, 218.37999, 94.821, 209.849, 93.770004, 191.59);
        arm_left.lineTo(93.68001, 191.26999);
        arm_left.curveTo(97.670006, 190.20999, 101.04001, 188.76999, 103.40101, 186.96999);
        arm_left.curveTo(103.560005, 186.84999, 103.711006, 186.73099, 103.86101, 186.59999);
        arm_left.lineTo(104.25101, 187.04);
        arm_left.curveTo(113.51101, 191.4, 114.72101, 199.35999, 110.991005, 205.31);
        arm_left.curveTo(111.17, 201.85, 98.71, 198.78, 90.33, 211.91);
        arm_left.closePath();
        
        
        hand_left = new GeneralPath();
        hand_left.moveTo(90.33, 211.91);
        hand_left.curveTo(98.71, 198.78, 111.17, 201.849, 110.990005, 205.309);
        hand_left.curveTo(109.950005, 207.009, 108.490005, 208.54901, 106.700005, 209.789);
        hand_left.curveTo(99.090004, 211.739, 93.9, 215.199, 87.3, 218.659);
        hand_left.lineTo(90.380005, 211.939);
        hand_left.lineTo(90.33, 211.91);
        hand_left.closePath();

        hand_right = new GeneralPath();
        hand_right.moveTo(110.99, 45.59);
        hand_right.curveTo(111.17, 49.05, 98.71, 52.12, 90.33, 38.99);
        hand_right.lineTo(90.380005, 38.960003);
        hand_right.lineTo(87.3, 32.24);
        hand_right.curveTo(93.901, 35.7, 99.090004, 39.160004, 106.700005, 41.11);
        hand_right.curveTo(108.49, 42.35, 109.95, 43.89, 110.99, 45.59);
        hand_right.closePath();
  }
    
    public void paint(Graphics2D g) 
    {
        g.setPaint(new Color(180, 180, 180, 255));
        g.fill(knee_left);
        g.fill(knee_right);
        
        g.setPaint(new Color(200, 200, 200, 255));
        g.fill(arm_left);
        g.fill(arm_right);
        
        g.setPaint(color);
        g.fill(shoulder_left); 
        g.fill(shoulder_right);
        g.fill(hand_left); 
        g.fill(hand_right);
        
        // stroke 
        g.setStroke(new BasicStroke(0.5f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND, 4.0f));
        g.setColor(Color.black);
        
        g.draw(knee_left);
        g.draw(knee_right);
        
        g.draw(arm_left);
        g.draw(arm_right);
        
        g.draw(shoulder_left); 
        g.draw(shoulder_right);
        g.draw(hand_left); 
        g.draw(hand_right);
        
        if(true)
        {
            double centerOffsetX = Nao_Simple.getOrigX() + Nao_Simple.getOrigWidth()/2;
            double centerOffsetY = Nao_Simple.getOrigY() + Nao_Simple.getOrigHeight()/2;

            g.translate(centerOffsetX, centerOffsetY);
            g.rotate(headYaw);
            g.translate(-centerOffsetX, -centerOffsetY);

            // fill
            g.setPaint(color);
            g.fill(head_outer); 
            g.fill(head_inner);
            
            g.setPaint(new Color(220, 220, 220, 255));
            g.fill(ear_left); 
            g.fill(ear_right);
            
            // stroke
            g.setColor(Color.black);
            g.draw(head_outer); 
            g.draw(head_inner);
            
            g.setColor(Color.black);
            g.draw(ear_left); 
            g.draw(ear_right);

            g.translate(centerOffsetX, centerOffsetY);
            g.rotate(-headYaw);
            g.translate(-centerOffsetX, -centerOffsetY);
        }//end head rotate
    }

    /**
     * Returns the X of the bounding box of the original SVG image.
     * 
     * @return The X of the bounding box of the original SVG image.
     */
    public static int getOrigX() {
        return 9;
    }

    /**
     * Returns the Y of the bounding box of the original SVG image.
     * 
     * @return The Y of the bounding box of the original SVG image.
     */
    public static int getOrigY() {
        return 24;
    }

    /**
     * Returns the width of the bounding box of the original SVG image.
     * 
     * @return The width of the bounding box of the original SVG image.
     */
    public static int getOrigWidth() {
        return 132;
    }

    /**
     * Returns the height of the bounding box of the original SVG image.
     * 
     * @return The height of the bounding box of the original SVG image.
     */
    public static int getOrigHeight() {
        return 205;
    }
}

