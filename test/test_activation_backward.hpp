#ifndef TEST_ACTIVATION_BACKWARD_HPP
#define TEST_ACTIVATION_BACKWARD_HPP

#include "hipdnn_test_common.h"

struct activation_params_t {
  int n, channels, height, width;
  activation_params_t(int n, int channels, int height, int width)
      : n(n), channels(channels), height(height), width(width) {}
};

template <typename dataType>
void compute_hipdnn_activation_backward(activation_params_t &test_case,
                                        dataType *src, dataType *grad,
                                        dataType *dst, float *avg_time) {
  hipdnnHandle_t hipdnn;
  checkHIPDNN(hipdnnCreate(&hipdnn));
  hipdnnTensorDescriptor_t in_desc;
  checkHIPDNN(hipdnnCreateTensorDescriptor(&in_desc));
  checkHIPDNN(hipdnnSetTensor4dDescriptor(
      in_desc, HIPDNN_TENSOR_NCHW, HIPDNN_DATA_FLOAT, test_case.n,
      test_case.channels, test_case.height, test_case.width));
  hipdnnActivationDescriptor_t activationDesc;
  //hipdnnActivationMode_t mode = HIPDNN_ACTIVATION_SIGMOID;
  hipdnnActivationMode_t mode = HIPDNN_ACTIVATION_RELU;
  hipdnnNanPropagation_t reluNanOpt = HIPDNN_NOT_PROPAGATE_NAN;
  double reluCeilingOrAlpha = 0;
  double activBeta = 0;
  double activExp = 0;
  checkHIPDNN(hipdnnCreateActivationDescriptor(&activationDesc));
  checkHIPDNN(hipdnnSetActivationDescriptor(activationDesc, mode, reluNanOpt,
                                            reluCeilingOrAlpha, activBeta,
                                            activExp));

  hipdnnTensorDescriptor_t out_desc;
  checkHIPDNN(hipdnnCreateTensorDescriptor(&out_desc));
  checkHIPDNN(hipdnnSetTensor4dDescriptor(
      out_desc, HIPDNN_TENSOR_NCHW, HIPDNN_DATA_FLOAT, test_case.n,
      test_case.channels, test_case.height, test_case.width));
  float alpha = 1.f;
  float beta = 0.f;

  hipdnnActivationForward(hipdnn, activationDesc, &alpha, in_desc, src, &beta,
                          out_desc, dst);

  high_resolution_timer_t timer;
    std::vector<double> time_vector(benchmark_iterations, 0);
    for (int i = 0; i < benchmark_iterations; i++) {

        timer.restart();
        hipdnnActivationBackward(hipdnn, activationDesc, &alpha, in_desc, src,
                           in_desc, src, out_desc, dst, &beta, out_desc, grad);
        hipDeviceSynchronize();
        std::uint64_t time_elapsed = timer.elapsed_nanoseconds();
        time_vector[i] = (double)time_elapsed / 1000;
    }

  *avg_time = (float)std::accumulate(time_vector.begin() + 10, time_vector.end(), 0) / (benchmark_iterations - 10);

  hipdnnDestroyTensorDescriptor(out_desc);
  hipdnnDestroyActivationDescriptor(activationDesc);
  hipdnnDestroyTensorDescriptor(in_desc);
  hipdnnDestroy(hipdnn);
}

#endif //TEST_ACTIVATION_BACKWARD_HPP
