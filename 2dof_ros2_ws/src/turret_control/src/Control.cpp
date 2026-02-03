#include "turret_control/Control.h"

Observer::Observer(StateSpace &ss_ref, std::shared_ptr<jsonRead> config) : ss(ss_ref), configReader(config)
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
    observer = std::make_unique<Observer>(ss, configReader);
    lqr = std::make_unique<LQR>(ss, configReader);
}

void TurretController::run()
{
Eigen::VectorXd y1;
  Eigen::VectorXd y2;
  Eigen::VectorXd u;
  y1.setZero(2);
  u.resize(2);
  u<< 10,10;

  std::cout << observer->estimate(u, y1) << "\n";
  std::cout << observer->estimate(u, y1) << "\n";
}

LQR::LQR(StateSpace &ss_ref, std::shared_ptr<jsonRead> config)
{
}