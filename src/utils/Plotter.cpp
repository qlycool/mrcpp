/*
 * MRCPP, a numerical library based on multiresolution analysis and
 * the multiwavelet basis which provide low-scaling algorithms as well as
 * rigorous error control in numerical computations.
 * Copyright (C) 2019 Stig Rune Jensen, Jonas Juselius, Luca Frediani and contributors.
 *
 * This file is part of MRCPP.
 *
 * MRCPP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MRCPP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MRCPP.  If not, see <https://www.gnu.org/licenses/>.
 *
 * For information on the complete list of contributors to MRCPP, see:
 * <https://mrcpp.readthedocs.io/>
 */

#include "Plotter.h"
#include "Printer.h"
#include "functions/RepresentableFunction.h"
#include "math_utils.h"
#include "trees/FunctionTree.h"
#include "trees/MWNode.h"

using namespace Eigen;

namespace mrcpp {

/** Plotter constructor

    Upper bound is placed BEFORE lower bound in argument list .
    Arguments:
    npts:	    points in each direction, default 1000
    *b:			upper bound, default (0, 0, ... , 0)
    *a:			lower bound, default (0, 0, ... , 0)
*/
template <int D>
Plotter<D>::Plotter(int npts, const double *a, const double *b)
        : fout(nullptr)
        , nPoints(npts) {
    setRange(a, b);
    setSuffix(Plotter<D>::Line, ".line");
    setSuffix(Plotter<D>::Surface, ".surf");
    setSuffix(Plotter<D>::Cube, ".cube");
    setSuffix(Plotter<D>::Grid, ".grid");
}

/** Set both bounds in one go

    If either bound is a NULL pointer, its value is set to (0, 0, ..., 0)

    Arguments:
    *a:			lower bound, default (0, 0, ... , 0)
    *b:			upper bound, default (0, 0, ... , 0)
*/
template <int D> void Plotter<D>::setRange(const double *a, const double *b) {
    for (int d = 0; d < D; d++) {
        if (a == nullptr) {
            A[d] = 0.0;
        } else {
            A[d] = a[d];
        }
        if (b == nullptr) {
            B[d] = 0.0;
        } else {
            B[d] = b[d];
        }
    }
}

/** Set number of plotting points

    The number of points is restricted to be the same in all directions
*/
template <int D> void Plotter<D>::setNPoints(int npts) {
    if (npts <= 0) MSG_ERROR("Invalid number of points");
    this->nPoints = npts;
}

/** Set file extension for output file

    The file name you decide for the output will get a predefined suffix
    that differentiates between different types of plot.

    Default values:
    line:    ".line"
    surface: ".surf"
    cube:    ".cube"
    grid:    ".grid"
*/
template <int D> void Plotter<D>::setSuffix(int t, const std::string &s) {
    this->suffix.insert(std::pair<int, std::string>(t, s));
}

/** Parametric plot of a function

    Plots the function func parametrically between the endpoints A and B,
    to a file named fname + file extension (".line" as default). Endpoints and
    the number of points must have been set at this point (default values are
    1000 points between the boundaries of the function).
*/
template <int D> void Plotter<D>::linePlot(const RepresentableFunction<D> &func, const std::string &fname) {
    println(20, "----------Line Plot-----------");
    if (not verifyRange()) MSG_ERROR("Zero range");
    calcLineCoordinates();
    evaluateFunction(func);
    std::stringstream file;
    file << fname << this->suffix[Plotter<D>::Line];
    openPlot(file.str());
    writeLineData();
    closePlot();
    printout(20, std::endl);
}

/** Surface plot of a function

    Plots the function func in 2D between the endpoints A and B, to a file
    named fname + file extension (".surf" as default). Endpoints and
    the number of points must have been set at this point (default values are
    1000 points between the boundaries of the function). The nPoints points
    will be distributed evenly in the two dimensions.
*/
template <int D> void Plotter<D>::surfPlot(const RepresentableFunction<D> &func, const std::string &fname) {
    println(20, "--------Surface Plot----------");
    if (not verifyRange()) MSG_ERROR("Zero range");
    calcSurfCoordinates();
    evaluateFunction(func);
    std::stringstream file;
    file << fname << this->suffix[Plotter<D>::Surface];
    openPlot(file.str());
    writeSurfData();
    closePlot();
    printout(20, std::endl);
}

/** Cubic plot of a function

    Plots the function func in 3D between the endpoints A and B, to a file
    named fname + file extension (".cube" as default). Endpoints and
    the number of points must have been set at this point (default values are
    1000 points between the boundaries of the function). The nPoints points
    will be distributed evenly in the three dimensions.
*/
template <int D> void Plotter<D>::cubePlot(const RepresentableFunction<D> &func, const std::string &fname) {
    println(20, "----------Cube Plot-----------");
    if (not verifyRange()) MSG_ERROR("Zero range");
    calcCubeCoordinates();
    evaluateFunction(func);
    std::stringstream file;
    file << fname << this->suffix[Plotter<D>::Cube];
    openPlot(file.str());
    writeCubeData();
    closePlot();
    printout(20, std::endl);
}

/** Parametric plot of a FunctionTree

    Plots the function func parametrically between the endpoints A and B,
    to a file named fname + file extension (".line" as default). Endpoints and
    the number of points must have been set at this point (default values are
    1000 points between the boundaries of the function).
*/
template <int D> void Plotter<D>::linePlot(FunctionTree<D> &tree, const std::string &fname) {
    println(20, "----------Line Plot-----------");
    if (not verifyRange()) MSG_ERROR("Zero range");
    calcLineCoordinates();
    evaluateFunction(tree);
    std::stringstream file;
    file << fname << this->suffix[Plotter<D>::Line];
    openPlot(file.str());
    writeLineData();
    closePlot();
    printout(20, std::endl);
}

/** Surface plot of a function

    Plots the function func in 2D between the endpoints A and B, to a file
    named fname + file extension (".surf" as default). Endpoints and
    the number of points must have been set at this point (default values are
    1000 points between the boundaries of the function). The nPoints points
    will be distributed evenly in the two dimensions.
*/
template <int D> void Plotter<D>::surfPlot(FunctionTree<D> &tree, const std::string &fname) {
    println(20, "--------Surface Plot----------");
    if (not verifyRange()) MSG_ERROR("Zero range");
    calcSurfCoordinates();
    evaluateFunction(tree);
    std::stringstream file;
    file << fname << this->suffix[Plotter<D>::Surface];
    openPlot(file.str());
    writeSurfData();
    closePlot();
    printout(20, std::endl);
}

/** Cubic plot of a function

    Plots the function func in 3D between the endpoints A and B, to a file
    named fname + file extension (".cube" as default). Endpoints and
    the number of points must have been set at this point (default values are
    1000 points between the boundaries of the function). The nPoints points
    will be distributed evenly in the three dimensions.
*/
template <int D> void Plotter<D>::cubePlot(FunctionTree<D> &tree, const std::string &fname) {
    println(20, "----------Cube Plot-----------");
    if (not verifyRange()) MSG_ERROR("Zero range");
    calcCubeCoordinates();
    evaluateFunction(tree);
    std::stringstream file;
    file << fname << this->suffix[Plotter<D>::Cube];
    openPlot(file.str());
    writeCubeData();
    closePlot();
    printout(20, std::endl);
}
/** Grid plot of a MWTree

    Writes a file named fname + file extension (".grid" as default) to be read
    by geomview to visualize the grid (of endNodes)	where the multiresolution
    function is defined. In MPI, each process will write a separate file, and
    will print only	nodes owned by itself (pluss the rootNodes).
*/
template <int D> void Plotter<D>::gridPlot(const MWTree<D> &tree, const std::string &fname) {
    println(20, "----------Grid Plot-----------");
    std::stringstream file;
    file << fname << this->suffix[Plotter<D>::Grid];
    openPlot(file.str());
    writeGrid(tree);
    closePlot();
    printout(20, std::endl);
}

/** Parametric plot of a function

    Plots the function func parametrically between the endpoints A and B,
    and returns a vector of function values. Endpoints and
    the number of points must have been set at this point (default values are
    1000 points between the boundaries of the function).
*/
template <int D> Eigen::VectorXd &Plotter<D>::linePlot(RepresentableFunction<D> &func) {
    calcLineCoordinates();
    evaluateFunction(func);
    return this->values;
}

/** Surface plot of a function

    Plots the function func in 2D between the endpoints A and B, and returns
    a vector of function values. Endpoints and
    the number of points must have been set at this point (default values are
    1000 points between the boundaries of the function). The nPoints points
    will be distributed evenly in the two dimensions.
*/
template <int D> Eigen::VectorXd &Plotter<D>::surfPlot(RepresentableFunction<D> &func) {
    calcSurfCoordinates();
    evaluateFunction(func);
    return this->values;
}

/** Cubic plot of a function

    Plots the function func in 3D between the endpoints A and B, and returns
    a vector of function values. Endpoints and
    the number of points must have been set at this point (default values are
    1000 points between the boundaries of the function). The nPoints points
    will be distributed evenly in the three dimensions.
*/
template <int D> Eigen::VectorXd &Plotter<D>::cubePlot(RepresentableFunction<D> &func) {
    calcCubeCoordinates();
    evaluateFunction(func);
    return this->values;
}

/** Calculating coordinates to be evaluated

    Generating a vector of nPoints equidistant coordinates that makes up the
    straight line between A and B in D dimensions. Coordiates are stored in
    the matrix coords for later evaluation.
*/
template <int D> void Plotter<D>::calcLineCoordinates() {
    double step[D];

    if (this->nPoints <= 0) {
        MSG_ERROR("Invalid number of points for plotting");
        return;
    }
    this->coords = MatrixXd::Zero(this->nPoints, D);
    for (int d = 0; d < D; d++) { step[d] = (this->B[d] - this->A[d]) / (this->nPoints + 1); }
    for (int i = 0; i < this->nPoints; i++) {
        for (int d = 0; d < D; d++) { this->coords(i, d) = this->A[d] + (i + 1) * step[d]; }
    }
}

template <int D> void Plotter<D>::calcSurfCoordinates() {
    NOT_IMPLEMENTED_ABORT;
    //    if (D != 2) {
    //        MSG_ERROR("Cannot plot planes for dim != 2!");
    //        return;
    //    }

    //    int nPerDim = (int) std::floor(std::sqrt(this->nPoints));
    //    int nRealPoints = math_utils::ipow(nPerDim, 2);
    //    this->coords = MatrixXd::Zero(nRealPoints, 2);

    //    double step[2];
    //    for (int d = 0; d < D; d++) {
    //        step[d] = (this->B[d] - this->A[d]) / (nPerDim + 1);
    //    }

    //    int n = 0;
    //    for (int i = 1; i <= nPerDim; i++) {
    //        for (int j = 1; j <= nPerDim; j++) {
    //            this->coords(n, 0) = i * step[0] + this->A[0];
    //            this->coords(n, 1) = j * step[1] + this->A[1];
    //            n++;
    //        }
    //    }
}

// Specialized for D=3 below
template <int D> void Plotter<D>::calcCubeCoordinates() {
    NOT_IMPLEMENTED_ABORT
}

/** Evaluating a function in a set of predfined coordinates

    Given that the set of coordinates ("coords") has been calculated, this
    routine evaluates the function in these points and stores the results
    in the vector "values".
*/
template <int D> void Plotter<D>::evaluateFunction(const RepresentableFunction<D> &func) {
    int totNPoints = this->coords.rows();
    if (not(totNPoints > 0)) {
        MSG_ERROR("Coordinates not set, cannot evaluate");
        return;
    }
    Coord<D> r;
    this->values = VectorXd::Zero(totNPoints);
    for (int i = 0; i < totNPoints; i++) {
        for (int d = 0; d < D; d++) { r[d] = this->coords(i, d); }
        this->values[i] = func.evalf(r);
    }
}

/** Evaluating a FunctionTree in a set of predfined coordinates

    Given that the set of coordinates ("coords") has been calculated, this
    routine evaluates the function in these points and stores the results
    in the vector "values".
*/
template <int D> void Plotter<D>::evaluateFunction(FunctionTree<D> &tree) {
    int totNPoints = this->coords.rows();
    if (not(totNPoints > 0)) {
        MSG_ERROR("Coordinates not set, cannot evaluate");
        return;
    }
    Coord<D> r;
    this->values = VectorXd::Zero(totNPoints);
    for (int i = 0; i < totNPoints; i++) {
        for (int d = 0; d < D; d++) { r[d] = this->coords(i, d); }
        this->values[i] = tree.evalf(r);
    }
}

/** Writing plot data to file

    This will write the contents of the "coords" matrix along with the function
    values to the file stream fout.	File will contain on each line the point
    number (between 0 and nPoints),	coordinates 1 through D and the function
    value.
*/
template <int D> void Plotter<D>::writeLineData() {
    std::ostream &o = *this->fout;
    int totNPoints = this->coords.rows();
    for (int i = 0; i < totNPoints; i++) {
        o.precision(8);
        o.setf(std::ios::showpoint);
        for (int d = 0; d < D; d++) { o << this->coords(i, d) << " "; }
        o.precision(12);
        //        o << ", ";
        o << this->values[i];
        //        o << ", ";
        //        o << i;
        o << std::endl;
    }
}

template <int D> void Plotter<D>::writeSurfData() {
    NOT_IMPLEMENTED_ABORT
}

// Specialized for D=3 below
template <int D> void Plotter<D>::writeCubeData() {
    NOT_IMPLEMENTED_ABORT
}

// Specialized for D=3 below
template <int D> void Plotter<D>::writeNodeGrid(const MWNode<D> &node, const std::string &color) {
    NOT_IMPLEMENTED_ABORT
}

// Specialized for D=3 below
template <int D> void Plotter<D>::writeGrid(const MWTree<D> &tree) {
    NOT_IMPLEMENTED_ABORT
}

/** Opening file for output

    Opens a file output stream fout for file named fname.
*/
template <int D> void Plotter<D>::openPlot(const std::string &fname) {
    if (fname.empty()) {
        if (this->fout == nullptr) {
            MSG_ERROR("Plot file not set!");
            return;
        } else if (this->fout->fail()) {
            MSG_ERROR("Plot file not set!");
            return;
        }
    } else {
        if (this->fout != nullptr) { this->fout->close(); }
        this->fout = &this->fstrm;
        this->fout->open(fname.c_str());
        if (this->fout->bad()) {
            MSG_ERROR("File error");
            return;
        }
    }
}

/** Closing file

    Closes the file output stream fout.
*/
template <int D> void Plotter<D>::closePlot() {
    if (this->fout != nullptr) this->fout->close();
    this->fout = nullptr;
}

/** Checks the validity of the plotting range
 */
template <int D> bool Plotter<D>::verifyRange() {
    for (int d = 0; d < D; d++) {
        if (this->A[d] > this->B[d]) { return false; }
    }
    return true;
}

/** Calculating coordinates to be evaluated

    Generating a vector of nPoints coordinates equally distributed between the
    dimensions, that fills the space of a cube defined by the lower- and upper
    corner (A and B). Coordiates are stored in the matrix coords for later
    evaluation.
*/
template <> void Plotter<3>::calcCubeCoordinates() {
    auto nPerDim = (int)std::floor(std::cbrt(this->nPoints));
    int nRealPoints = math_utils::ipow(nPerDim, 3);
    this->coords = MatrixXd::Zero(nRealPoints, 3);

    double step[3];
    for (int d = 0; d < 3; d++) { step[d] = (this->B[d] - this->A[d]) / (nPerDim - 1); }

    int n = 0;
    for (int i = 0; i < nPerDim; i++) {
        for (int j = 0; j < nPerDim; j++) {
            for (int k = 0; k < nPerDim; k++) {
                this->coords(n, 0) = i * step[0] + this->A[0];
                this->coords(n, 1) = j * step[1] + this->A[1];
                this->coords(n, 2) = k * step[2] + this->A[2];
                n++;
            }
        }
    }
}

/** Writing plot data to file

    This will write a cube file (readable by jmol) of the function values
    previously calculated (the "values" vector).
*/
template <> void Plotter<3>::writeCubeData() {
    int np = 0;
    double max = -1.0e10;
    double min = 1.0e10;
    double isoval = 0.e0;
    double step[3];

    std::ofstream &o = *this->fout;

    o << "Cube file format. Generated by MRCPP.\n" << std::endl;

    auto nPerDim = (int)std::floor(std::cbrt(this->nPoints));
    int nRealPoints = math_utils::ipow(nPerDim, 3);

    for (int d = 0; d < 3; d++) { step[d] = (this->B[d] - this->A[d]) / (nPerDim - 1); }

    //	"%5d %12.6f %12.6f %12.6f\n"
    o.setf(std::ios::scientific);
    o.precision(12);
    o << 0 << " " << 0.0 << " " << 0.0 << " " << 0.0 << std::endl;
    o << nPerDim << " " << step[0] << " " << 0.0 << " " << 0.0 << std::endl;
    o << nPerDim << " " << 0.0 << " " << step[1] << " " << 0.0 << std::endl;
    o << nPerDim << " " << 0.0 << " " << 0.0 << " " << step[2] << std::endl;

    o << std::endl;
    for (int n = 0; n < nRealPoints; n++) {
        o << this->values[n] << " "; // 12.5E
        if (n % 6 == 5) o << std::endl;
        if (this->values[n] < min) min = this->values[n];
        if (this->values[n] > max) max = this->values[n];
        double p = std::abs(this->values[n]);
        if (p > 1.e-4 && p < 1.e+2) {
            np += 1;
            isoval += p;
        }
    }

    isoval = isoval / np;
    println(0, "Max value:" << max);
    println(0, "Min value:" << min);
    println(0, "Isovalue: " << isoval);
}

template <> void Plotter<3>::writeNodeGrid(const MWNode<3> &node, const std::string &color) {
    double origin[3] = {0, 0, 0};
    double length = std::pow(2.0, -node.getScale());
    std::ostream &o = *this->fout;

    for (int d = 0; d < 3; d++) { origin[d] = node.getTranslation()[d] * length; }
    o << origin[0] << " " << origin[1] << " " << origin[2] << " " << color << origin[0] << " " << origin[1] << " "
      << origin[2] + length << " " << color << origin[0] << " " << origin[1] + length << " " << origin[2] + length
      << " " << color << origin[0] << " " << origin[1] + length << " " << origin[2] << color << std::endl;

    o << origin[0] << " " << origin[1] << " " << origin[2] << " " << color << origin[0] << " " << origin[1] << " "
      << origin[2] + length << " " << color << origin[0] + length << " " << origin[1] << " " << origin[2] + length
      << " " << color << origin[0] + length << " " << origin[1] << " " << origin[2] << color << std::endl;
    o << origin[0] << " " << origin[1] << " " << origin[2] << " " << color << origin[0] << " " << origin[1] + length
      << " " << origin[2] << " " << color << origin[0] + length << " " << origin[1] + length << " " << origin[2] << " "
      << color << origin[0] + length << " " << origin[1] << " " << origin[2] << color << std::endl;

    o << origin[0] + length << " " << origin[1] + length << " " << origin[2] + length << " " << color
      << origin[0] + length << " " << origin[1] + length << " " << origin[2] << " " << color << origin[0] + length
      << " " << origin[1] << " " << origin[2] << " " << color << origin[0] + length << " " << origin[1] << " "
      << origin[2] + length << color << std::endl;

    o << origin[0] + length << " " << origin[1] + length << " " << origin[2] + length << " " << color
      << origin[0] + length << " " << origin[1] + length << " " << origin[2] << " " << color << origin[0] << " "
      << origin[1] + length << " " << origin[2] << " " << color << origin[0] << " " << origin[1] + length << " "
      << origin[2] + length << color << std::endl;

    o << origin[0] + length << " " << origin[1] + length << " " << origin[2] + length << " " << color
      << origin[0] + length << " " << origin[1] << " " << origin[2] + length << " " << color << origin[0] << " "
      << origin[1] << " " << origin[2] + length << " " << color << origin[0] << " " << origin[1] + length << " "
      << origin[2] + length << color << std::endl;
}

/** Writing grid data to file

    This will write a grid file (readable by geomview) of the grid (of endNodes)
    where the multiresolution function is defined.
    Currently only working in 3D.
*/
template <> void Plotter<3>::writeGrid(const MWTree<3> &tree) {
    std::ostream &o = *this->fout;
    o << "CQUAD" << std::endl;
    o.precision(6);
    std::string rootColor = " 1 1 1 0 ";
    std::string color = " 0 0 1 1 ";
    for (int i = 0; i < tree.getRootBox().size(); i++) {
        const MWNode<3> &rootNode = tree.getRootMWNode(i);
        writeNodeGrid(rootNode, rootColor);
    }
    for (int i = 0; i < tree.getNEndNodes(); i++) {
        const MWNode<3> &node = tree.getEndMWNode(i);
        writeNodeGrid(node, color);
    }
}

template class Plotter<1>;
template class Plotter<2>;
template class Plotter<3>;

} // namespace mrcpp
