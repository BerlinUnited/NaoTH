package de.naoth.rc.opengl.representations;

import com.jogamp.opengl.GLException;
import com.jogamp.opengl.math.FloatUtil;

/**
 * Simple float array-backed float 4x4 matrix
 * exposing {@link FloatUtil} matrix functionality
 */
public class Matrix4 {
	
    protected float[] matrix;  
    
    public Matrix4() {
          matrix = new float[16];
          loadIdentity();
    }
    
    public Matrix4(float[] matrix) {
        this.matrix = matrix;
  }

    public float[] get() {
        return matrix;
    }
    
    public void setMatrix(float[] matrix) {
        this.matrix = matrix;
    }

    public void loadIdentity() {
        FloatUtil.makeIdentity(matrix);
    }

    /**
     * Multiply matrix: [this] = [this] x [m]
     * @param m 4x4 matrix in column-major order
     */
    public void multMatrix(final float[] m) {
        FloatUtil.multMatrix(matrix, m);
    }

    /**
     * Multiply matrix: [this] = [this] x [m]
     * @param m 4x4 matrix in column-major order
     */
    public void multMatrix(final Matrix4 m) {
        FloatUtil.multMatrix(matrix, m.matrix);
    }

    /**
     * @param v_in 4-component column-vector
     * @param v_out this * v_in
     */
    public float[] multVec(final float[] v_in) {
    	float[] v_out = new float[4];
        FloatUtil.multMatrixVec(matrix, v_in, v_out);
        return v_out;
    }
    
    /**
     * @param v_in 3-component column-vector
     * @param w 4th component
     * @return this * (v_in, w)
     */
    public float[] multVec(float[] v_in, float w) {
    	float[] v = new float[]{v_in[0], v_in[1], v_in[2], w};
    	float[] v_out = new float[4];
        FloatUtil.multMatrixVec(matrix, v, v_out);
        return new float[]{v_out[0],v_out[1],v_out[2]};
    }
    
    /**
     * @param v_in 3-component column-vector
     * @param w 4th component
     * @return this * (v_in, w)
     */
    public Vector3f multVec(Vector3f v_in, float w) {
    	float[] v = new float[]{v_in.getX(), v_in.getY(), v_in.getZ(), w};
    	float[] v_out = new float[4];
        FloatUtil.multMatrixVec(matrix, v, v_out);
        return new Vector3f(v_out);
    }

    public void translate(final float x, final float y, final float z) {
    	float[] tempMat = new float[16];
        multMatrix(FloatUtil.makeTranslation(tempMat, false, x, y, z));
    }

    public void scale(final float x, final float y, final float z) {
    	float[] tempMat = new float[16];
        multMatrix(FloatUtil.makeScale(tempMat, false, x, y, z));
    }

    public void rotate(final float angrad, final float x, final float y, final float z) {
    	float[] tempMat = new float[16];
    	float[] tempVec = new float[4];
        multMatrix(FloatUtil.makeRotationAxis(tempMat, 0, angrad, x, y, z, tempVec));
    }

    public float determinant() {
        return FloatUtil.matrixDeterminant(matrix);
    }

    public boolean invert() {
    	float[] tempMat = matrix.clone();
        return null != FloatUtil.invertMatrix(matrix, tempMat);
    }
    
    /**
     * @param fovy_rad
     * @param aspect
     * @param zNear
     * @param zFar
     * @throws GLException if {@code zNear <= 0} or {@code zFar <= zNear}
     * @see FloatUtil#makePerspective(float[], int, boolean, float, float, float, float)
     */
    public void makePerspective(final float fovy_rad, final float aspect, final float zNear, final float zFar) throws GLException {
    	float[] tempMat = new float[16];
    	matrix = FloatUtil.makePerspective(tempMat, 0, true, fovy_rad, aspect, zNear, zFar);
    }
    
    public void makeOrtho(final float left, final float right, final float bottom, final float top, final float zNear, final float zFar) throws GLException {
    	float[] tempMat = new float[16];
    	matrix = FloatUtil.makeOrtho(tempMat, 0, true, left, right, bottom, top, zNear, zFar);
    }
    
    public void makeRotation(final float ang, final float x, final float y, final float z) throws GLException {
    	float[] tempMat = new float[16];
    	float[] tempVec = new float[4];

    	matrix = FloatUtil.makeRotationAxis(tempMat, 0, ang, x, y, z, tempVec);
    }

    /*
    public void makeLookAt(Vector3f eye, Vector3f center, Vector3f up) throws GLException {
    	float[] tempMat = new float[16];
    	float[] tempMat2 = new float[16];
    	
    	matrix = FloatUtil.makeLookAt(tempMat, 0, eye.get(), 0, center.get(), 0, up.get(), 0, tempMat2);
    }
    */
    
	public void makeLookAt(Vector3f eye, Vector3f center, Vector3f up) {
		up.normalize();
		
		Vector3f f =  new Vector3f(center.get().clone());
		f.subVec(eye);
		f.normalize();
				
		Vector3f s = new Vector3f(up.get().clone());
		s.crossVec(f);
		s.normalize();
		
		Vector3f u = new Vector3f(s.get().clone());
		u.crossVec(f);
		
		//create VIEW matrix
		matrix = new float[16];
		matrix[0] = s.getX();  matrix[4] = s.getY();  matrix[8] = s.getZ();   matrix[12] = -s.dotVec(eye);    
		matrix[1] = u.getX();  matrix[5] = u.getY();  matrix[9] = u.getZ();   matrix[13] = -u.dotVec(eye);       
		matrix[2] = -f.getX(); matrix[6] = -f.getY(); matrix[10] = -f.getZ(); matrix[14] = f.dotVec(eye); 
		matrix[3] = 0f;        matrix[7] = 0f;        matrix[11] = 0f;        matrix[15] = 1f;
	}
    
    public String toString() {
    	return
    	matrix[0]+" "+matrix[4]+" "+matrix[6] +" "+matrix[12]+"\n"+
		matrix[1]+" "+matrix[5]+" "+matrix[9] +" "+matrix[13]+"\n"+
		matrix[2]+" "+matrix[6]+" "+matrix[10]+" "+matrix[14]+"\n"+
		matrix[3]+" "+matrix[7]+" "+matrix[11]+" "+matrix[15]+"\n";
    }
    
    public Matrix4 clone() {
    	return new Matrix4(this.matrix.clone());
    }
    
    public void transpose() {
        float[] tempMat = new float[16];
        FloatUtil.transposeMatrix(this.matrix, tempMat);
        this.matrix = tempMat;
    }
    
}