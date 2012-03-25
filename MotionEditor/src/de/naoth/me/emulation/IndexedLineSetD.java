/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;
import js.math.vector.Vec3d;
import java.util.ArrayList;

/**
 *
 * @author claas
 */
public class IndexedLineSetD extends GeometryD
{
  public ArrayList<Vec3d> coords;
  public ArrayList<Integer> coordIndexes;

  public IndexedLineSetD()
  {
    coords = new ArrayList<Vec3d>();
    coordIndexes = new ArrayList<Integer>();
  }

  public IndexedLineSetD(String n)
  {
    coords = new ArrayList<Vec3d>();
    coordIndexes = new ArrayList<Integer>();
    name = n;
  }
}
