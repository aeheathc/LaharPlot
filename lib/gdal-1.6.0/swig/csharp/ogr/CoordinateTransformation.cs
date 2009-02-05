/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.36
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace OSGeo.OGR {

using System;
using System.Runtime.InteropServices;

public class CoordinateTransformation : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;
  protected object swigParentRef;
  
  protected static object ThisOwn_true() { return null; }
  protected object ThisOwn_false() { return this; }

  public CoordinateTransformation(IntPtr cPtr, bool cMemoryOwn, object parent) {
    swigCMemOwn = cMemoryOwn;
    swigParentRef = parent;
    swigCPtr = new HandleRef(this, cPtr);
  }

  public static HandleRef getCPtr(CoordinateTransformation obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }
  public static HandleRef getCPtrAndDisown(CoordinateTransformation obj, object parent) {
    if (obj != null)
    {
      obj.swigCMemOwn = false;
      obj.swigParentRef = parent;
      return obj.swigCPtr;
    }
    else
    {
      return new HandleRef(null, IntPtr.Zero);
    }
  }
  public static HandleRef getCPtrAndSetReference(CoordinateTransformation obj, object parent) {
    if (obj != null)
    {
      obj.swigParentRef = parent;
      return obj.swigCPtr;
    }
    else
    {
      return new HandleRef(null, IntPtr.Zero);
    }
  }

  ~CoordinateTransformation() {
    Dispose();
  }

  public virtual void Dispose() {
  lock(this) {
      if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
        swigCMemOwn = false;
        OsrPINVOKE.delete_CoordinateTransformation(swigCPtr);
      }
      swigCPtr = new HandleRef(null, IntPtr.Zero);
      swigParentRef = null;
      GC.SuppressFinalize(this);
    }
  }

  public CoordinateTransformation(SpatialReference src, SpatialReference dst) : this(OsrPINVOKE.new_CoordinateTransformation(SpatialReference.getCPtr(src), SpatialReference.getCPtr(dst)), true, null) {
    if (OsrPINVOKE.SWIGPendingException.Pending) throw OsrPINVOKE.SWIGPendingException.Retrieve();
  }

  public void TransformPoint(double[] inout) {
    OsrPINVOKE.CoordinateTransformation_TransformPoint__SWIG_0(swigCPtr, inout);
    if (OsrPINVOKE.SWIGPendingException.Pending) throw OsrPINVOKE.SWIGPendingException.Retrieve();
  }

  public void TransformPoint(double[] argout, double x, double y, double z) {
    OsrPINVOKE.CoordinateTransformation_TransformPoint__SWIG_1(swigCPtr, argout, x, y, z);
    if (OsrPINVOKE.SWIGPendingException.Pending) throw OsrPINVOKE.SWIGPendingException.Retrieve();
  }

  public void TransformPoints(int nCount, double[] x, double[] y, double[] z) {
    OsrPINVOKE.CoordinateTransformation_TransformPoints(swigCPtr, nCount, x, y, z);
    if (OsrPINVOKE.SWIGPendingException.Pending) throw OsrPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
