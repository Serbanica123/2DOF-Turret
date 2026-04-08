#include "turret_control/Control.h"

Observer::Observer(std::shared_ptr<jsonRead> config, StateSpace &ss_ref) : configReader(config), ss(ss_ref)
{
    this->L = configReader->readMatrix({"observer", "L"});

    this->x_hat.setZero(ss.n);
}

Eigen::VectorXd &Observer::estimate(const Eigen::VectorXd &u, const Eigen::VectorXd &y)

{
    if (u.size() != ss.m || y.size() != ss.p)
    {
        std::cerr << "[Observer] Dimension mismatch\n";
        return x_hat;
    }
    x_hat = ss.A * x_hat + ss.B * u + L * (y - ss.C * x_hat);
    return x_hat;
}
TurretController::TurretController(const char *docName)
{
    configReader = std::make_shared<jsonRead>(docName);

    ss.setSystem(configReader->readInt({"dims", "n"}),
                 configReader->readInt({"dims", "m"}),
                 configReader->readInt({"dims", "p"}),
                 configReader->readDouble({"model", "dt"}),
                 configReader->readMatrix({"model", "A"}),
                 configReader->readMatrix({"model", "B"}),
                 configReader->readMatrix({"model", "C"}),
                 configReader->readMatrix({"model", "D"}));
    observer = std::make_unique<Observer>(configReader, ss);
    lqr = std::make_unique<LQR>(configReader, ss);
    u_prev.setZero(ss.m);
}

TurretController::TurretController()
{
}
Eigen::VectorXd TurretController::run(const Eigen::VectorXd &y)
{

    Eigen::VectorXd x_hat = observer->estimate(u_prev, y);

    Eigen::VectorXd u = lqr->computeControls(x_hat);

    u_prev = u;

    return u;
}

LQR::LQR(std::shared_ptr<jsonRead> config, StateSpace &ss_ref) : configReader(config), ss(ss_ref)
{
    this->x_ss.setZero(ss.n);
    this->u_ss.setZero(ss.m);
    this->K = configReader->readMatrix({"lqr", "K"});
}

void LQR::updateReference(const Eigen::VectorXd &r)
{
    Eigen::MatrixXd M(ss.n + ss.p, ss.n + ss.m);
    Eigen::VectorXd rhs(ss.n + ss.p);

    M << Eigen::MatrixXd::Identity(ss.n, ss.n) - ss.A, -ss.B,
        ss.C, Eigen::MatrixXd::Zero(ss.p, ss.m);
    rhs << Eigen::VectorXd::Zero(ss.n), r;

    Eigen::VectorXd sol = M.fullPivLu().solve(rhs);
    x_ss = sol.head(ss.n);
    u_ss = sol.tail(ss.m);
}

Eigen::VectorXd LQR::computeControls(const Eigen::VectorXd &x_hat)
{
    return u_ss - K * (x_hat - x_ss);
}

double TurretController::getDt()
{
    return ss.dt;
}

int TurretController::getOutputNum()
{
    return ss.p;
}

void TurretController::updateReference(const Eigen::VectorXd &r)
{
    lqr->updateReference(r);
}