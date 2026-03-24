#ifndef CONTROL_H
#define CONTROL_H
#include "turret_control/JsonRead.h"

struct StateSpace
{
    Eigen::Index n, m, p;
    double dt;

    Eigen::MatrixXd A;
    Eigen::MatrixXd B;
    Eigen::MatrixXd C;
    Eigen::MatrixXd D;

    StateSpace() : n(0), m(0), p(0), dt(0.0)
    {
        A = Eigen::MatrixXd(0, 0);
        B = Eigen::MatrixXd(0, 0);
        C = Eigen::MatrixXd(0, 0);
        D = Eigen::MatrixXd(0, 0);
    }

    StateSpace(const size_t n_, const size_t m_, const size_t p_, const double dt_, const Eigen::MatrixXd &A_, const Eigen::MatrixXd &B_, const Eigen::MatrixXd &C_, const Eigen::MatrixXd &D_)
        : n(n_), m(m_), p(p_), dt(dt_),
          A(A_), B(B_), C(C_), D(D_)
    {
        if (n == 0 || m == 0 || p == 0)
            throw std::invalid_argument("StateSpace dimensions must be > 0");

        if (dt <= 0.0)
            throw std::invalid_argument("dt must be > 0");
    }

    void setSystem(size_t n_, size_t m_, size_t p_, double dt_,
                   const Eigen::MatrixXd &A_,
                   const Eigen::MatrixXd &B_,
                   const Eigen::MatrixXd &C_,
                   const Eigen::MatrixXd &D_)
    {
        n = n_;
        m = m_;
        p = p_;
        dt = dt_;
        A = A_;
        B = B_;
        C = C_;
        D = D_;
    }

    friend std::ostream &operator<<(std::ostream &os, const StateSpace &ss)
    {
        os << "StateSpace:\n";
        os << "n = " << ss.n << ", m = " << ss.m << ", p = " << ss.p << ", dt = " << ss.dt << "\n\n";

        os << "A =\n"
           << ss.A << "\n\n";
        os << "B =\n"
           << ss.B << "\n\n";
        os << "C =\n"
           << ss.C << "\n\n";
        os << "D =\n"
           << ss.D << "\n";

        return os;
    }

};

class Observer
{
public:
    Observer(std::shared_ptr<jsonRead> config, StateSpace &ss_ref);
    Eigen::VectorXd &estimate(const Eigen::VectorXd &u, const Eigen::VectorXd &y);

private:
    std::shared_ptr<jsonRead> configReader;

    StateSpace &ss;
    Eigen::MatrixXd L;
    Eigen::VectorXd x_hat;
};

class LQR
{
public:
    LQR(std::shared_ptr<jsonRead> config, StateSpace &ss_ref);
    void updateReference(const Eigen::VectorXd &r);
    Eigen::VectorXd computeControls(const Eigen::VectorXd &x_hat);

private:
    std::shared_ptr<jsonRead> configReader;
    StateSpace &ss;
    Eigen::MatrixXd K;
    Eigen::VectorXd x_ss;
    Eigen::VectorXd u_ss;

};

class TurretController
{
public:
    TurretController(const char *docName);
    TurretController();
    Eigen::VectorXd run(const Eigen::VectorXd &y);
    double getDt();
    int getOutputNum();
private:
    StateSpace ss;
    std::shared_ptr<jsonRead> configReader;
    std::unique_ptr<Observer> observer;
    std::unique_ptr<LQR> lqr;

    Eigen::VectorXd u_prev;
    Eigen::VectorXd last_reference;

};

#endif
