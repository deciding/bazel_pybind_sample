#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>

#include "eigen_utils.hpp"

namespace py = pybind11;

namespace {

int add(int x, int y) { return x + y; }

} // namespace

class Matrix {
public:
    Matrix(){}
    Matrix(size_t rows, size_t cols) : m_rows(rows), m_cols(cols) {
        m_data = new float[rows*cols];
    }
    Matrix(size_t rows, size_t cols, float* d) : m_rows(rows), m_cols(cols), m_data(d) {
    }
    float *data() { return m_data; }
    size_t rows() const { return m_rows; }
    size_t cols() const { return m_cols; }
private:
    size_t m_rows, m_cols;
    float *m_data;
};

class NoCopyWrapper {
public:
    NoCopyWrapper(){}
    //NoCopyWrapper(py::array_t<float> &arr): raw(arr){
    //NoCopyWrapper(py::array_t<float>* arr){
    NoCopyWrapper(py::array_t<float> arr, int N, int M){
    //NoCopyWrapper(float* arr, int N, int M){
        //auto x = arr.mutable_unchecked<2>();
        //mat = Matrix(x.shape(0), x.shape(1), x.mutable_data());
        //mat = Matrix(arr.shape(0), arr.shape(1), arr.mutable_data());

        //raw = arr;
        py::str dummyDataOwner;
        raw = py::array_t<float>(
            {N, M}, // shape
            {M*sizeof(float), sizeof(float)}, // C-style contiguous strides for double
            arr.data(), dummyDataOwner);
    };

    NoCopyWrapper(py::array_t<double> list){
        vec = list;
    }

    //Matrix get_mat() { return mat; }
    void create_mat(){
        mat.resize(1000, 1000);
        for(int i = 0; i< 1000; i++)
            for(int j = 0; j< 1000; j++)
                mat(i, j) = 255;
    }
    ColMatrix<uint8_t>& get_mat() { return mat; }
    py::array_t<float>& get_raw() { return raw; }
    py::array_t<double>& get_vec() { return vec; }
private:
    //Matrix mat;
    //py::array_t<float> &raw;
    py::array_t<float> raw;
    py::array_t<double> vec;
    ColMatrix<uint8_t> mat;
};

PYBIND11_MODULE(my_pb_mod, m) {

    m.def("add", &add, "adds two numbers");

    //py::class_<Matrix>(m, "Matrix", py::buffer_protocol())
    //   .def_buffer([](Matrix &m) -> py::buffer_info {
    //        return py::buffer_info(
    //            m.data(),                               /* Pointer to buffer */
    //            sizeof(float),                          /* Size of one scalar */
    //            py::format_descriptor<float>::format(), /* Python struct-style format descriptor */
    //            2,                                      /* Number of dimensions */
    //            { m.rows(), m.cols() },                 /* Buffer dimensions */
    //            { sizeof(float) * m.cols(),             /* Strides (in bytes) for each index */
    //              sizeof(float) }
    //        );
    //    });

    py::class_<NoCopyWrapper>(m, "NoCopyWrapper")
        .def(py::init<py::array_t<float>, int, int>())
        .def(py::init<py::array_t<double>>())
        .def(py::init())
        .def("create_mat", &NoCopyWrapper::create_mat)
        .def("get_mat", &NoCopyWrapper::get_mat, py::return_value_policy::reference_internal)
        .def("get_raw", &NoCopyWrapper::get_raw, py::return_value_policy::reference_internal)
        .def("get_vec", &NoCopyWrapper::get_vec, py::return_value_policy::reference_internal);
}
