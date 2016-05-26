#include "PointCloud.hpp"

namespace Ra
{
    namespace Core
    {
        namespace PointCloud
        {

            inline Vector3 meanPoint(const Vector3Array& pts)
            {
                return pts.getMap().rowwise().mean();
            }

            inline Transform principalAxis(const Vector3Array& pts)
            {
                Transform result;

                Vector3 meanPt = meanPoint(pts);
                result.translation() = meanPt;

                // Subtract points from their average.
                Vector3Array ptsAvg(pts.size());
                ptsAvg.getMap() = pts.getMap().colwise() - meanPt;
                CORE_ASSERT(Math::areApproxEqual(meanPoint(ptsAvg).squaredNorm(), 0.f), "oops");

                // Compute variance-covariance matrix
                auto vCov = (1.f / (pts.size() - 1)) * (ptsAvg.getMap() * ptsAvg.getMap().transpose());

                // Solve eigen vectors
                Eigen::SelfAdjointEigenSolver<Matrix3> solver(vCov);
                result.linear() = solver.eigenvectors();
                return result;
            }

            Obb pcaObb(const Vector3Array& pts)
            {
                Transform pca = principalAxis(pts);
                Transform pcaInv = pca.inverse();

                // Compute the AABB in principal axis space.
                Vector3Array alignedPts;
                alignedPts.reserve(pts.size());

                // TODO : is there an eigen trick to do that without a loop ?
                for (const auto& v : pts)
                {
                    alignedPts.push_back(pcaInv * v);
                }

                Aabb aligned = aabb(alignedPts);
                return Obb(aligned, pca);

            }

            Aabb aabb(const Vector3Array& pts)
            {
                return pts.size() > 0 ?
                       Aabb(pts.getMap().rowwise().minCoeff(),
                            pts.getMap().rowwise().maxCoeff()) :
                       Aabb();
            }


        }
    }
}
