package de.naoth.rc.opengl.representations;

import com.jogamp.opengl.math.VectorUtil;

public class Vector3f {
	
	protected float[] vector;
	
	public Vector3f() {
		vector = new float[3];
		vector[0] = vector[1] = vector[2] = 0;	
	}
	
	public Vector3f(float x, float y, float z) {
		vector = new float[3];
		vector[0] = x;
		vector[1] = y;
		vector[2] = z;
	}
	
	public Vector3f(float[] vector) {
		this.vector = new float[3];
		this.vector[0] = vector[0];
		this.vector[1] = vector[1];
		this.vector[2] = vector[2];
	}
	
	public Vector3f(Point3f point) {
		vector = point.get();
	}
	
	public void normalize() {
		VectorUtil.normalizeVec3(vector);
	}
	
	public void addVec(Vector3f vector) {
		float[] tempVec = new float[3]; 
		VectorUtil.addVec3(tempVec, this.vector, vector.get());
		this.vector = tempVec;
	}
	
	public void subVec(Vector3f vector) {
		float[] tempVec = new float[3]; 
		VectorUtil.subVec3(tempVec, this.vector, vector.get());
		this.vector = tempVec;
	}
	
	public void mulFloat(float multiplier) {
		this.vector[0] = multiplier * this.vector[0];
		this.vector[1] = multiplier * this.vector[1];
		this.vector[2] = multiplier * this.vector[2];		
	}
	
	public void crossVec(Vector3f vector) {
		float[] tempVec = new float[3]; 
		VectorUtil.crossVec3(tempVec, this.vector, vector.get());
		this.vector = tempVec;
	}
	
	public float distVec(Vector3f vector) {
		return VectorUtil.distVec3(this.vector, vector.get());
	}
	
	public float angleVec(Vector3f vector) {
		return VectorUtil.angleVec3(this.vector, vector.get());
	}
	
	public float dotVec(Vector3f vector) {
		return VectorUtil.dotVec3(this.vector, vector.get());
	}
	
	public void rotateAroundAxis(float angrad, float x, float y, float z) {
		Matrix4 rotMat = new Matrix4();
		rotMat.makeRotation(angrad, x, y, z);
		
		this.vector = rotMat.multVec(this.vector, 0);
	}
	
	public void rotateAroundPoint(Vector3f rotationPoint, float angrad, float x, float y, float z) {
		this.subVec(rotationPoint);
		this.rotateAroundAxis(angrad, x, y, z);
		this.addVec(rotationPoint);
	}
	
	public float[] get() {
		return vector;
	}
	
	public float getX() {
		return vector[0];
	}
	
	public float getY() {
		return vector[1];
	}
	
	public float getZ() {
		return vector[2];
	}
	
	public String toString(){
		return "(" + vector[0] + " ," + vector[1] + " ," + vector[2] + ")";
	}
	
	public Vector3f clone(){
		return new Vector3f(this.vector.clone());
	}
}
