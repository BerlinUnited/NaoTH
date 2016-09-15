package de.naoth.rc.opengl;

import de.naoth.rc.opengl.representations.Matrix4;
import de.naoth.rc.opengl.representations.Point3f;
import de.naoth.rc.opengl.representations.Vector3f;

/**
 * Camera with different modes to move around the scene
 */
public class Camera {
	final public static char FLY_MODE = 1;
	final public static char FOCUS_MODE = 2;
	
	private Matrix4 cameraMatrix;
	
	private Matrix4 projectionMatrix;
	private Matrix4 viewMatrix;
	
	private Vector3f pos, focus, up;
	
	private Vector3f right, direction;
	
	private char mode;
	
    /**
     * Creates a new Camera object.
     * @param mode FLY_MODE, FOCUS_MODE
     * @param pos Position of the camera in world coordinates
     * @param focus target position the camera should look at
     * @param mode FLY_MODE, FOCUS_MODE
     * @param up specifies there is up, usually (0,1,0)
     */	
	public Camera(char mode, Point3f pos, Point3f focus, Point3f up, int width, int height) {
		this.pos = new Vector3f(pos);
		this.focus = new Vector3f(focus);
		this.up = new Vector3f(up);
		
		setMode(mode);
		
		viewMatrix = new Matrix4();
		viewMatrix.makeLookAt(this.pos, this.focus, this.up);
		
		projectionMatrix = new Matrix4();
		projectionMatrix.makePerspective(50, (float)width / (float)height, 0.1f, 1000f);
		
		cameraMatrix = projectionMatrix.clone();
		cameraMatrix.multMatrix(viewMatrix);
	}
	
    /**
     * changes the type of moving
     * @param mode FLY_MODE, FOCUS_MODE
     */	
	public void setMode(char mode) {
		if(this.mode == mode) return;
		
		this.mode = mode;
		
		switch(mode) {
		
		case FLY_MODE:
			this.focus.subVec(this.pos);
			this.focus.normalize();
			this.focus.addVec(this.pos);
			break;
			
		case FOCUS_MODE:
			break;
		}
	}
	
    /**
     * @param focus target position the camera should look at
     */	
	public void setFocus(Vector3f focus) {
		this.focus = focus;
		setMode(FOCUS_MODE);
	}
	
	private void calcRight() {	
		this.right = up.clone();
		this.right.crossVec(direction);		
	}
	
	private void calcDirection() {
		direction = pos.clone();
		direction.subVec(this.focus);
		direction.normalize();	
	}
	
	public Matrix4 getCameraMatrix() {
		return cameraMatrix; 
	}
	
	public Vector3f getPos() {
		return this.pos;
	}
	
    /**
     * Needs to be called if the display resolution was changed,
     * to recalculate the projection matrix.
     */	
	public void reshape(int width, int height) {
		projectionMatrix = new Matrix4();
		projectionMatrix.makePerspective(50, (float)width / (float)height, 0.1f, 1000f);
		
		cameraMatrix = projectionMatrix.clone();
		cameraMatrix.multMatrix(viewMatrix);
	}
	
    /**
     * Needs to be called if the camera was moved,
     * to recalculate the view matrix.
     */
	public void review() {
		viewMatrix.makeLookAt(this.pos, this.focus, this.up);

		cameraMatrix = projectionMatrix.clone();
		cameraMatrix.multMatrix(viewMatrix);
	}
	
	public void moveForward(float speed) {
		calcDirection();
		
		Vector3f newPos = pos.clone();
		
		Vector3f step = direction.clone();
		
		step.mulFloat(speed * -1);
		newPos.addVec(step);
		
		if(this.mode == FOCUS_MODE){
			if(newPos.distVec(focus)<1){
				return;
			}
			else this.pos = newPos;
		}
		if(this.mode == FLY_MODE) {
			this.pos = newPos;
			this.focus.addVec(step);
		}
	}
	
	public void rotateRight(float angrad) {
		if(this.mode == FOCUS_MODE) {			
			pos.subVec(focus);
			pos.rotateAroundAxis(angrad, 0, 1, 0);
			pos.addVec(focus);
		}
		else if(this.mode == FLY_MODE) {
			focus.subVec(pos);
			focus.rotateAroundAxis(-angrad, 0, 1, 0);
			focus.addVec(pos);
		}
	}
	
	public void rotateUp(float angrad) {
		calcDirection();
		calcRight();
		
		Vector3f movedPoint;
		Vector3f newDirection;
		
		if(this.mode == FOCUS_MODE) {
			movedPoint = pos.clone();
			
			movedPoint.rotateAroundPoint(focus, angrad, right.getX(), right.getY(), right.getZ());
			
			newDirection = movedPoint.clone();
			newDirection.subVec(this.focus);
			
			if(Math.abs(newDirection.angleVec(up)-Math.toRadians(90)) > Math.toRadians(80)) {
				return;
			}
			else {
				pos = movedPoint;
			}
		}
		else if(this.mode == FLY_MODE) {
			movedPoint = focus.clone();
			
			movedPoint.rotateAroundPoint(pos, angrad, right.getX(), right.getY(), right.getZ());
			
			newDirection = movedPoint.clone();
			newDirection.subVec(this.pos);
			
			if(Math.abs(newDirection.angleVec(up)-Math.toRadians(90)) > Math.toRadians(80)) {
				return;
			}
			else {
				focus = movedPoint;
			}
		}
	}

	public void moveRight(float speed) {
		// TODO Auto-generated method stub
		
	}
}
