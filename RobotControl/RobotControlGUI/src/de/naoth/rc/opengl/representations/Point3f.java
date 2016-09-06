package de.naoth.rc.opengl.representations;

public class Point3f {
	public float x, y, z;
	
	public Point3f() {
		x = y = z = 0;
	}
	
	public Point3f(float[] points) {
		this.x = points[0];
		this.y = points[1];
		this.z = points[2];
	}
	
	public Point3f(float x, float y, float z) {
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public float[] get() {
		return new float[] {x, y, z};
	}
}
