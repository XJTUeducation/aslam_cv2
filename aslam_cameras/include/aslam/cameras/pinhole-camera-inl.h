#ifndef ASLAM_CAMERAS_PINHOLE_CAMERA_INL_H_
#define ASLAM_CAMERAS_PINHOLE_CAMERA_INL_H_

#include <memory>

namespace aslam {

// TODO(dymczykm) actually, I'm not sure if it wouldn't be better if
// specialized versions of these function (for double) would use this
// implementation instead of repeating it twice.

template <typename ScalarType, typename DistortionType>
bool PinholeCamera::euclideanToKeypoint(
    const Eigen::Matrix<ScalarType, 3, 1>& point,
    Eigen::Matrix<ScalarType, 2, 1>* out_point) const {
  CHECK_NOTNULL(out_point);

  ScalarType rz = static_cast<ScalarType>(1.0) / point[2];
  Eigen::Matrix<ScalarType, 2, 1> keypoint;
  keypoint[0] = point[0] * rz;
  keypoint[1] = point[1] * rz;

  CHECK_NOTNULL(_distortion.get());
  std::shared_ptr<DistortionType> distortion_ptr =
      std::dynamic_pointer_cast<DistortionType>(_distortion);
  CHECK(distortion_ptr);
  distortion_ptr->distort(keypoint, out_point);

  (*out_point)[0] = static_cast<ScalarType>(_fu) * (*out_point)[0]
      + static_cast<ScalarType>(_cu);
  (*out_point)[1] = static_cast<ScalarType>(_fv) * (*out_point)[1]
      + static_cast<ScalarType>(_cv);

  return isValid(*out_point) && point[2] > static_cast<ScalarType>(0);
}

template <typename ScalarType>
bool PinholeCamera::isValid(
    const Eigen::Matrix<ScalarType, 2, 1>& keypoint) const {
  return keypoint[0] >= static_cast<ScalarType>(0)
      && keypoint[1] >= static_cast<ScalarType>(0)
      && keypoint[0] < static_cast<ScalarType>(_ru)
      && keypoint[1] < static_cast<ScalarType>(_rv);
}

}  // namespace aslam

#endif  // ASLAM_CAMERAS_PINHOLE_CAMERA_INL_H_
