/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * KneadEmulator.java
 *
 * Created on 07.11.2009, 13:02:57
 *
 */

package de.naoth.me.emulation;

import de.naoth.me.core.*;

import java.awt.GraphicsConfiguration;
import java.awt.GraphicsEnvironment;
import java.awt.Rectangle;

import java.io.FileNotFoundException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.media.j3d.*;
import javax.vecmath.*;

import com.sun.j3d.loaders.*;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import org.jdesktop.j3d.loaders.vrml97.VrmlLoader;
import javax.swing.JPanel;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import javax.swing.JOptionPane;

import com.sun.j3d.utils.geometry.Text2D;

import java.awt.Font;

/**
 *
 * @author claas
 */
public class KneadEmulator extends JPanel implements PropertyChangeListener
{

    private VirtualUniverse universe = null;
    private BoundingSphere globalBounds = null;
    private Locale locale = null;
    private Map<String, TransformGroup> namedObjects;
    private Map<String, BranchGroup> namedGroupObjects;

    private TransformGroup coordinationTG = null;
    private Canvas3D canvas_nao;

    private Vector3d worldMiddle = null;
    private Vector3d headOrigin = null;
    private Vector3d lShoulderOrigin = null;
    private Vector3d rShoulderOrigin = null;
    private Vector3d elbowOrigin = null;
    private Vector3d lHipOrigin = null;
    private Vector3d rHipOrigin = null;
    private Vector3d kneeOrigin = null;
    private Vector3d ankleOrigin = null;
    private Vector3d textOrigin = null;

    private JointConfiguration config = null;;
    private Shape3D infoText = null;
    private OrbitBehavior orbit = null;

    private Transform3D t3d = new Transform3D();

    /** Creates new form KneadEmulator */
    public KneadEmulator() throws java.lang.UnsatisfiedLinkError
    {
      try
      {
        globalBounds = new BoundingSphere(new Point3d(0, 0, 0), Double.MAX_VALUE);
        locale = null;
        coordinationTG = new TransformGroup();

        worldMiddle = new Vector3d(0.0, 0.0, 0.0);
        headOrigin = new Vector3d(0.0, 0.0, 0.125);
        lShoulderOrigin = new Vector3d(0.0, 0.1, 0.1);
        rShoulderOrigin = new Vector3d(0.0, -0.1, 0.1);
        elbowOrigin = new Vector3d(0.1, 0.0, 0.0);
        lHipOrigin = new Vector3d(0.005, 0.05, -0.085);
        rHipOrigin = new Vector3d(0.005, -0.05, -0.085);
        kneeOrigin = new Vector3d(0.0, 0.0, -0.1);
        ankleOrigin = new Vector3d(0.0, 0.0, -0.1);
        textOrigin = new Vector3d(1.4460, -0.21, 0.16);

        GraphicsConfigTemplate3D template = new GraphicsConfigTemplate3D();

        GraphicsConfiguration gconf = GraphicsEnvironment.getLocalGraphicsEnvironment()
          .getDefaultScreenDevice().getBestConfiguration(template);

        canvas_nao = new Canvas3D(gconf);

        initComponents();

        this.add(canvas_nao);
        canvas_nao.setSize(570,453);

        createUniverse();
        createViewBranch(canvas_nao, new Vector3f(2, 0, 0));
        createSceneGraphic();
      }
      catch(java.lang.UnsatisfiedLinkError e)
      {
        JOptionPane.showMessageDialog
        (
          this,
          "Java 3D is not installed!\nVisit https://java3d.dev.java.net\n" + e.toString(),
          "Java 3D is not installed",
          JOptionPane.ERROR_MESSAGE
        );
        throw e;
      }
//      catch (Exception e)
//      {
//        JOptionPane.showMessageDialog
//        (
//          this,
//          "Can not initialize Java 3D\n",
//          e.toString(),
//          JOptionPane.ERROR_MESSAGE
//        );
//      }
    }

    public void setConfig(JointConfiguration newConfig)
    {
      if(newConfig == null)
      {
        config = new JointConfiguration(new JointDefaultConfiguration());
      }
      else
      {
        config = newConfig;
      }
    }

    public void destroy()
    {
        universe.removeAllLocales();
    }

    private void createUniverse()
    {
        universe = new VirtualUniverse();
        locale = new Locale(universe);
    }

    private void createSceneGraphic()
    {
      namedObjects = new HashMap<String, TransformGroup>();
      namedGroupObjects = new HashMap<String, BranchGroup>();
      Loader ldr = new VrmlLoader(Loader.LOAD_ALL);
      String modelPath = "./3d_models";

      String[] naofiles = new String[]
                          {
                            "Head",
                            "Chest",
                            "RHipYawPitch",
                            "RHipRoll",
                            "RThigh",
                            "RTibia",
                            "RAnkle",
                            "RFoot",
                            "LHipYawPitch",
                            "LHipRoll",
                            "LThigh",
                            "LTibia",
                            "LAnkle",
                            "LFoot",
                            "RUpperArm",
                            "RLowerArm",
                            "LUpperArm",
                            "LLowerArm"
                          };

      coordinationTG.setCapability(TransformGroup.ALLOW_CHILDREN_EXTEND);
      coordinationTG.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
      coordinationTG.setCapability(TransformGroup.ENABLE_PICK_REPORTING);
      BranchGroup coordinationBG = new BranchGroup();
      coordinationBG.addChild(coordinationTG);
      locale.addBranchGraph(coordinationBG);

			//load all nao parts
      BranchGroup nao = new BranchGroup();
      for (int i = 0; i < naofiles.length; i++)
      {
        BranchGroup bg = load(modelPath + "/nao/v3/" + naofiles[i] + ".wrl", ldr);
        namedGroupObjects.put(naofiles[i], bg);
      }

//      Appearance app = new Appearance();
//      Color3f lightGray = new Color3f(0.8f, 0.8f, 0.8f);
//      ColoringAttributes ca = new ColoringAttributes(lightGray, ColoringAttributes.NICEST);
//      app.setColoringAttributes(ca);
////      cameraLine.setCapability(LineArray.ALLOW_COORDINATE_WRITE);
//      Shape3D plShape = new Shape3D(cameraLine, app);

      //group all directly connected parts with chest
      TransformGroup tg = namedObjects.get("Chest");

      tg.addChild(namedGroupObjects.get("Head"));
      tg.addChild(namedGroupObjects.get("RHipYawPitch"));
      tg.addChild(namedGroupObjects.get("LHipYawPitch"));
      tg.addChild(namedGroupObjects.get("RUpperArm"));
      tg.addChild(namedGroupObjects.get("LUpperArm"));

      //group all directly connected parts with HipYawPitch right side ,...
      tg = namedObjects.get("RHipYawPitch");
      tg.addChild(namedGroupObjects.get("RHipRoll"));
      tg = namedObjects.get("RHipRoll");
      tg.addChild(namedGroupObjects.get("RThigh"));
      tg = namedObjects.get("RThigh");
      tg.addChild(namedGroupObjects.get("RTibia"));
      tg = namedObjects.get("RTibia");
      tg.addChild(namedGroupObjects.get("RAnkle"));
      tg = namedObjects.get("RAnkle");
      tg.addChild(namedGroupObjects.get("RFoot"));

      //group all directly connected parts with HipYawPitch left side ,...
      tg = namedObjects.get("LHipYawPitch");
      tg.addChild(namedGroupObjects.get("LHipRoll"));
      tg = namedObjects.get("LHipRoll");
      tg.addChild(namedGroupObjects.get("LThigh"));
      tg = namedObjects.get("LThigh");
      tg.addChild(namedGroupObjects.get("LTibia"));
      tg = namedObjects.get("LTibia");
      tg.addChild(namedGroupObjects.get("LAnkle"));
      tg = namedObjects.get("LAnkle");
      tg.addChild(namedGroupObjects.get("LFoot"));

      //group all directly connected parts with UpperArm right side
      tg = namedObjects.get("RUpperArm");
      tg.addChild(namedGroupObjects.get("RLowerArm"));

      //group all directly connected parts with UpperArm left side
      tg = namedObjects.get("LUpperArm");
      tg.addChild(namedGroupObjects.get("LLowerArm"));

      //add chest and all with it groupoed  parts to nao object
      nao.addChild(namedGroupObjects.get("Chest"));
//      AmbientLight lightA = new AmbientLight();
//      lightA.setInfluencingBounds(new BoundingSphere());
//      nao.addChild(lightA);

      MousePickingBehavior pick = new MousePickingBehavior(canvas_nao, nao, new BoundingSphere(), this);
      nao.addChild(pick);

      infoText = new Text2D("nothing selected", new Color3f(1f, 1f, 0f), "Serif", 20, Font.BOLD);
      tg = new TransformGroup();
      tg.addChild(infoText);
      tg.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
      tg.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
      tg.setCapability(Shape3D.ALLOW_APPEARANCE_READ);
      tg.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);

      namedObjects.put("2D Info", tg);

      Transform3D t3dText = new Transform3D();

      t3d = new Transform3D();
      t3d.rotX(Math.PI / 2);
      tg.getTransform(t3dText);
      t3d.mul(t3dText);
      t3dText = new Transform3D();
      t3dText.rotZ(Math.PI / 2);
      t3dText.mul(t3d);
      t3dText.setScale(0.3);
      t3d = new Transform3D();
      t3dText.setTranslation(textOrigin);
      tg.setTransform(t3dText);

      nao.addChild(tg);


      DirectionalLight lightD1 = new DirectionalLight();
      lightD1.setInfluencingBounds(new BoundingSphere());
      nao.addChild(lightD1);
      coordinationTG.addChild(nao);

      BranchGroup light = load(modelPath + "/env/Light.wrl", ldr);
      locale.addBranchGraph(light);

      setConfig(null);
      initJoints();

    }

    public void setIgnoreInput(Boolean ignore)
    {
      orbit.setIgnoreInput(ignore);
    }

    public BranchGroup getBranchGroup(String name)
    {
      return namedGroupObjects.get(name);
    }

    public void setInfoText(String text)
    {
      ((Text2D) infoText).setString(text);
    }

    public void textRotX(double angle)
    {
      if(namedObjects != null && namedObjects.containsKey("2D Info"))
      {
        Transform3D toRot = new Transform3D();
        TransformGroup targetTG =  namedObjects.get("2D Info");

        targetTG.getTransform(t3d);
        toRot.rotX(angle);
        toRot.mul(t3d);
        targetTG.setTransform(toRot);
      }
    }

    public void textRotY(double angle)
    {
      if(namedObjects != null && namedObjects.containsKey("2D Info"))
      {
        Transform3D toRot = new Transform3D();
        TransformGroup targetTG =  namedObjects.get("2D Info");

        targetTG.getTransform(t3d);
        toRot.rotY(angle);
        toRot.mul(t3d);
        targetTG.setTransform(toRot);
      }
    }
    public void textRotZ(double angle)
    {
      if(namedObjects != null && namedObjects.containsKey("2D Info"))
      {
        Transform3D toRot = new Transform3D();
        TransformGroup targetTG =  namedObjects.get("2D Info");

        targetTG.getTransform(t3d);
        toRot.rotZ(angle);
        toRot.mul(t3d);
        targetTG.setTransform(toRot);
      }
    }

    public void textMove(Vector3d theMove)
    {
      if(namedObjects != null && namedObjects.containsKey("2D Info"))
      {
        Transform3D toMove = new Transform3D();
        TransformGroup targetTG =  namedObjects.get("2D Info");
        targetTG.getTransform(t3d);
        t3d.setScale(1.0);
        toMove.setTranslation(theMove);
        t3d.mul(toMove);
        t3d.setScale(0.3);
        targetTG.setTransform(t3d);
      }
    }


    // load vrml file and return the contents BranchGroup
    // and add content as TranformGroup to namedObjects
    private BranchGroup load(String filename, Loader ldr)
    {
      BranchGroup bg = null;
      try
      {
        Scene vrml_scene = ldr.load(filename);
        bg = vrml_scene.getSceneGroup();
        java.util.Hashtable ht = vrml_scene.getNamedObjects();
        if (ht != null)
        {
          for (Enumeration e = ht.keys(); e.hasMoreElements();)
          {
            String name = (String) e.nextElement();
            Object to = ht.get(name);
            if ( TransformGroup.class.isInstance(to) )
            {
              TransformGroup tg = (TransformGroup) to;
              tg.setName(name);
              tg.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
              tg.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
              tg.setCapability(TransformGroup.ENABLE_PICK_REPORTING);
              tg.setCapability(Shape3D.ALLOW_APPEARANCE_READ);
              tg.setCapability(Shape3D.ALLOW_APPEARANCE_WRITE);
              namedObjects.put(name, tg);
            }
          }
        }
      }
      catch (FileNotFoundException ex)
      {
        Logger.getLogger(KneadEmulator.class.getName()).log(Level.SEVERE, null, ex);
      }
      catch (IncorrectFormatException ex)
      {
        Logger.getLogger(KneadEmulator.class.getName()).log(Level.SEVERE, null, ex);
      }
      catch (ParsingErrorException ex)
      {
        Logger.getLogger(KneadEmulator.class.getName()).log(Level.SEVERE, null, ex);
      }
      return bg;
    }

    private void createViewBranch(Canvas3D canvas, Vector3f homeViewPos)
    {
      BranchGroup viewBranch = new BranchGroup();

      TransformGroup viewTG = new TransformGroup();
      viewTG.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);

      ViewPlatform vp = new ViewPlatform();

      View view = new View();
      view.setPhysicalBody(new PhysicalBody());
      view.setPhysicalEnvironment(new PhysicalEnvironment());

      view.attachViewPlatform(vp);
      view.addCanvas3D(canvas);

      orbit = new OrbitBehavior(canvas, viewTG, view, this);
      orbit.setSchedulingBounds(globalBounds);
      orbit.setClippingEnabled(false);

      orbit.setViewingTransform(new Point3d(homeViewPos), new Point3d(0,0,0), new Vector3d(0,0,1), new Point3d(0,0,0));

      viewTG.addChild(vp);
      viewTG.addChild(orbit);

      viewBranch.addChild(viewTG);
      locale.addBranchGraph(viewBranch);
    }

    private void initJoints()
    {
      setHead(this.config.getJoint("HeadPitch").getRadiant(), this.config.getJoint("HeadYaw").getRadiant());

      setShoulder("L", this.config.getJoint("LShoulderPitch").getRadiant(), this.config.getJoint("LShoulderRoll").getRadiant());
      setShoulder("R", this.config.getJoint("RShoulderPitch").getRadiant(), this.config.getJoint("RShoulderRoll").getRadiant());

      setElbow("L", this.config.getJoint("LElbowYaw").getRadiant(), this.config.getJoint("LElbowRoll").getRadiant());
      setElbow("R", this.config.getJoint("RElbowYaw").getRadiant(), this.config.getJoint("RElbowRoll").getRadiant());

      setAnkleRoll("L", this.config.getJoint("LAnkleRoll").getRadiant());
      setAnklePitch("L", this.config.getJoint("LAnklePitch").getRadiant());
      setKneePitch("L", this.config.getJoint("LKneePitch").getRadiant());
      setHipPitch("L", this.config.getJoint("LHipPitch").getRadiant());
      setHipRoll("L", this.config.getJoint("LHipRoll").getRadiant());

      setAnkleRoll("R", this.config.getJoint("RAnkleRoll").getRadiant());
      setAnklePitch("R", this.config.getJoint("RAnklePitch").getRadiant());
      setKneePitch("R", this.config.getJoint("RKneePitch").getRadiant());
      setHipPitch("R", this.config.getJoint("RHipPitch").getRadiant());
      setHipRoll("R", this.config.getJoint("RHipRoll").getRadiant());

      setHipYawPitch(this.config.getJoint("LHipYawPitch").getRadiant());
    }

    public void propertyChange(PropertyChangeEvent event)
    {
      if(event.getPropertyName().equals("selectedKeyFrame") )
      {
          if(event.getNewValue() instanceof KeyFrame)
          {
            KeyFrame newKeyFrame = (KeyFrame) event.getNewValue();
            setConfig(newKeyFrame.getJointConfiguration());
          }
          else if(event.getNewValue() == null)
          {
            setConfig(null);
          }
          initJoints();
      }

      if(event.getPropertyName().equals("jointValueChanged"))
      {
        Joint joint = (Joint) event.getNewValue();

        if(joint.getId().equals("HeadPitch"))
        {
          setHead(joint.getRadiant(), config.getJoint("HeadYaw").getRadiant());
        }
        if(joint.getId().equals("HeadYaw"))
        {
          setHead(config.getJoint("HeadPitch").getRadiant(), joint.getRadiant());
        }

        if(joint.getId().equals("LShoulderPitch"))
        {
          setShoulder("L", joint.getRadiant(), config.getJoint("LShoulderRoll").getRadiant());
        }
        if(joint.getId().equals("RShoulderPitch"))
        {
          setShoulder("R", joint.getRadiant(), config.getJoint("RShoulderRoll").getRadiant());
        }

        if(joint.getId().equals("LShoulderRoll"))
        {
          setShoulder("L", config.getJoint("LShoulderPitch").getRadiant(), joint.getRadiant());
        }
        if(joint.getId().equals("RShoulderRoll"))
        {
          setShoulder("R", config.getJoint("RShoulderPitch").getRadiant(), joint.getRadiant());
        }

        if(joint.getId().equals("LElbowRoll"))
        {
          setElbow("L", config.getJoint("LElbowYaw").getRadiant(), joint.getRadiant());
        }
        if(joint.getId().equals("RElbowRoll"))
        {
          setElbow("R", config.getJoint("RElbowYaw").getRadiant(), joint.getRadiant());
        }

        if(joint.getId().equals("LElbowYaw"))
        {
          setElbow("L", joint.getRadiant(), config.getJoint("LElbowRoll").getRadiant());
        }
        if(joint.getId().equals("RElbowYaw"))
        {
          setElbow("R", joint.getRadiant(), config.getJoint("RElbowRoll").getRadiant());
        }

        if(joint.getId().equals("LHipYawPitch"))
        {
          setHipYawPitch(joint.getRadiant());
        }

        if(joint.getId().equals("RHipYawPitch"))
        {
          setHipYawPitch(joint.getRadiant());
        }

        if(joint.getId().equals("LHipRoll"))
        {
          setHipRoll("L", joint.getRadiant());
        }
        if(joint.getId().equals("RHipRoll"))
        {
          setHipRoll("R", joint.getRadiant());
        }

        if(joint.getId().equals("LHipPitch"))
        {
          setHipPitch("L", joint.getRadiant());
        }
        if(joint.getId().equals("RHipPitch"))
        {
          setHipPitch("R", joint.getRadiant());
        }

        if(joint.getId().equals("LKneePitch"))
        {
          setKneePitch("L", joint.getRadiant());
        }
        if(joint.getId().equals("RKneePitch"))
        {
          setKneePitch("R", joint.getRadiant());
        }

        if(joint.getId().equals("LAnklePitch"))
        {
          setAnklePitch("L", joint.getRadiant());
        }
        if(joint.getId().equals("RAnklePitch"))
        {
          setAnklePitch("R", joint.getRadiant());
        }

        if(joint.getId().equals("LAnkleRoll"))
        {
          setAnkleRoll("L", joint.getRadiant());
        }
        if(joint.getId().equals("RAnkleRoll"))
        {
          setAnkleRoll("R", joint.getRadiant());
        }

      }
    }//end propertyChange

    public void setHeadJoints( double pitch, double yaw)
    {
      Joint joint = config.getJoint("HeadPitch");
      pitch += joint.getRadiant();
      double angleD = getCheckedJointValues(joint, pitch * 180/ Math.PI);
      pitch = angleD / 180 * Math.PI;

      config.setJointValue("HeadPitch", angleD);

      joint = config.getJoint("HeadYaw");
      yaw += joint.getRadiant();
      angleD = getCheckedJointValues(joint, yaw * 180/ Math.PI);
      yaw = angleD / 180 * Math.PI;

      config.setJointValue("HeadYaw", angleD);
    }

    public void setHead( double pitch, double yaw)
    {
      Transform3D trans = new Transform3D();
      Transform3D transRot = new Transform3D();
      TransformGroup head = namedObjects.get("Head");

      head.getTransform(trans);
      trans.setTranslation(worldMiddle);
      trans.rotY(pitch);
      transRot.rotZ(yaw);
      transRot.mul(trans);
      transRot.setTranslation(headOrigin);
      head.setTransform(transRot);
    }

    public void setShoulderJoints(String side, double pitch, double roll)
    {
      Joint joint = config.getJoint(side + "ShoulderPitch");
      pitch += joint.getRadiant();
      double angleD = getCheckedJointValues(joint, pitch * 180/ Math.PI);
      pitch = angleD / 180 * Math.PI;

      config.setJointValue(side + "ShoulderPitch", angleD);

      joint = config.getJoint(side + "ShoulderRoll");
      roll += joint.getRadiant();
      angleD = getCheckedJointValues(joint, roll * 180/ Math.PI);
      roll = angleD / 180 * Math.PI;

      config.setJointValue(side + "ShoulderRoll", angleD);
    }

    public void setShoulder(String side, double pitch, double roll)
    {
      Transform3D trans = new Transform3D();
      Transform3D transRot = new Transform3D();
      TransformGroup shoulder = namedObjects.get(side + "UpperArm");

      shoulder.getTransform(trans);
      trans.setTranslation(worldMiddle);
      trans.rotZ(roll);
      transRot.rotY(pitch);
      transRot.mul(trans);
      if(side.equals("L"))
      {
        transRot.setTranslation(lShoulderOrigin);
      }
      else
      {
        transRot.setTranslation(rShoulderOrigin);
      }
      shoulder.setTransform(transRot);
    }

    public void setElbowJoints(String side, double roll, double yaw)
    {
      Joint joint = config.getJoint(side + "ElbowRoll");
      roll += joint.getRadiant();
      double angleD = getCheckedJointValues(joint, roll * 180/ Math.PI);
      roll = angleD / 180 * Math.PI;

      config.setJointValue(side + "ElbowRoll", angleD);

      joint = config.getJoint(side + "ElbowYaw");
      yaw += joint.getRadiant();
      angleD = getCheckedJointValues(joint, yaw * 180/ Math.PI);
      yaw = angleD / 180 * Math.PI;

      config.setJointValue(side + "ElbowYaw", angleD);
    }

    public void setElbow(String side, double roll, double yaw)
    {
      Transform3D trans = new Transform3D();
      Transform3D transRot = new Transform3D();
      TransformGroup arm = namedObjects.get(side + "LowerArm");

      arm.getTransform(trans);
      trans.setTranslation(worldMiddle);
      trans.rotZ(yaw);
      transRot.rotX(roll);
      transRot.mul(trans);
      transRot.setTranslation(elbowOrigin);
      arm.setTransform(transRot);
    }

    public void setAnkleRollJoint(String side, double angle)
    {
      Joint joint = config.getJoint(side + "AnkleRoll");
      angle += joint.getRadiant();
      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
      angle = angleD / 180 * Math.PI;

      config.setJointValue(side + "AnkleRoll", angle / Math.PI * 180);
    }

    public void setAnkleRoll(String side, double angle)
    {
      Transform3D footTrans = new Transform3D();
      TransformGroup footLeft = namedObjects.get(side + "Foot");

      footLeft.getTransform(footTrans);
      footTrans.setTranslation(worldMiddle);
      footTrans.rotX(angle);
      footLeft.setTransform(footTrans);
    }

    public void setAnklePitchJoint(String side, double angle)
    {
      Joint joint = config.getJoint(side + "AnklePitch");
      angle += joint.getRadiant();
      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
      angle = angleD / 180 * Math.PI;

      config.setJointValue(side + "AnklePitch", angle / Math.PI * 180);
    }

    public void setAnklePitch(String side, double angle)
    {
      Transform3D ankleTrans = new Transform3D();
      TransformGroup ankleLeft;

      ankleLeft= namedObjects.get(side + "Ankle");
      ankleLeft.getTransform(ankleTrans);
      ankleTrans.setTranslation(worldMiddle);
      ankleTrans.rotY(angle);
      ankleTrans.setTranslation(ankleOrigin);
      ankleLeft.setTransform(ankleTrans);
    }

    public void setKneePitchJoint(String side, double angle)
    {
      Joint joint = config.getJoint(side + "KneePitch");
      angle += joint.getRadiant();
      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
      angle = angleD / 180 * Math.PI;

      config.setJointValue(side + "KneePitch", angle / Math.PI * 180);
    }

    public void setKneePitch(String side, double angle)
    {
      Transform3D kneeTrans = new Transform3D();
      TransformGroup kneeLeft = namedObjects.get(side + "Tibia");

      kneeLeft.getTransform(kneeTrans);
      kneeTrans.setTranslation(worldMiddle);
      kneeTrans.rotY(angle);
      kneeTrans.setTranslation(kneeOrigin);
      kneeLeft.setTransform(kneeTrans);
    }

    public void setHipPitchJoint(String side, double angle)
    {
      Joint joint = config.getJoint(side + "HipPitch");
      angle += joint.getRadiant();
      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
      angle = angleD / 180 * Math.PI;

      config.setJointValue(side + "HipPitch", angle / Math.PI * 180);
    }

    public void setHipPitch(String side, double angle)
    {
      Transform3D hipTrans = new Transform3D();
      TransformGroup hipLeft = namedObjects.get(side + "Thigh");

      hipLeft.getTransform(hipTrans);
      hipTrans.setTranslation(worldMiddle);
      hipTrans.rotY(angle);
      hipLeft.setTransform(hipTrans);
    }

    public void setHipRollJoint(String side, double angle)
    {
      Joint joint = config.getJoint(side + "HipRoll");
      angle += joint.getRadiant();
      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
      angle = angleD / 180 * Math.PI;

      config.setJointValue(side + "HipRoll", angle / Math.PI * 180);
    }

    public void setHipRoll(String side, double angle)
    {
      Transform3D hipTrans = new Transform3D();
      TransformGroup hipLeft = namedObjects.get(side + "HipRoll");

      hipLeft.getTransform(hipTrans);
      hipTrans.setTranslation(worldMiddle);
      hipTrans.rotX(angle);
      hipLeft.setTransform(hipTrans);
    }

    public void setHipYawPitchJoints(double angle)
    {
      Joint joint = config.getJoint("LHipYawPitch");
      angle += joint.getRadiant();
      double angleD = getCheckedJointValues(joint, angle * 180/ Math.PI);
      angle = angleD / 180 * Math.PI;

      config.setJointValue("LHipYawPitch", angle / Math.PI * 180);

      config.setJointValue("RHipYawPitch", angle / Math.PI * 180);
      joint = config.getJoint("RHipYawPitch");
    }

    public void setHipYawPitch(double angle)
    {
      Transform3D hipTrans = new Transform3D();
      TransformGroup hip = namedObjects.get("LHipYawPitch");

      hip.getTransform(hipTrans);
      hipTrans.setTranslation(worldMiddle);
      AxisAngle4d axis = new AxisAngle4d(0, 1, -1, angle);
      hipTrans.setRotation(axis);
      hipTrans.setTranslation(lHipOrigin);
      hip.setTransform(hipTrans);

      hip = namedObjects.get("RHipYawPitch");

      hip.getTransform(hipTrans);
      hipTrans.setTranslation(worldMiddle);
      axis = new AxisAngle4d(0, 1, 1, angle);
      hipTrans.setRotation(axis);
      hipTrans.setTranslation(rHipOrigin);
      hip.setTransform(hipTrans);
    }

    private double getCheckedJointValues(Joint joint, double angle)
    {
      if(angle > joint.getMaxValue())
      {
        return joint.getMaxValue();
      }
      if(angle < joint.getMinValue())
      {
        return joint.getMinValue();
      }
      return angle;
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    setPreferredSize(new java.awt.Dimension(570, 453));
    addComponentListener(new java.awt.event.ComponentAdapter() {
      public void componentResized(java.awt.event.ComponentEvent evt) {
        formComponentResized(evt);
      }
    });

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
    this.setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGap(0, 570, Short.MAX_VALUE)
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGap(0, 453, Short.MAX_VALUE)
    );
  }// </editor-fold>//GEN-END:initComponents

    private void formComponentResized(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_formComponentResized
      Rectangle bounds = this.getBounds();
//      bounds.height -= 80;
      canvas_nao.setBounds(bounds);
      canvas_nao.repaint();
    }//GEN-LAST:event_formComponentResized


  // Variables declaration - do not modify//GEN-BEGIN:variables
  public final javax.swing.JPopupMenu jPopupMenuSelect = new javax.swing.JPopupMenu();
  // End of variables declaration//GEN-END:variables

}
