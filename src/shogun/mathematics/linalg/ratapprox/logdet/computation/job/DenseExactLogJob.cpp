/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Heiko Strathmann, Soumyajit De, Björn Esser
 */

#include <shogun/lib/config.h>

#include <shogun/lib/SGVector.h>
#include <shogun/lib/SGMatrix.h>
#include <shogun/lib/computation/jobresult/ScalarResult.h>
#include <shogun/mathematics/eigen3.h>
#include <shogun/mathematics/linalg/linop/DenseMatrixOperator.h>
#include <shogun/mathematics/linalg/ratapprox/logdet/computation/job/DenseExactLogJob.h>

using namespace Eigen;

namespace shogun
{

CDenseExactLogJob::CDenseExactLogJob()
	: CIndependentJob()
{
	init();

	SG_GCDEBUG("%s created (%p)\n", this->get_name(), this)
}

CDenseExactLogJob::CDenseExactLogJob(CJobResultAggregator* aggregator,
	CDenseMatrixOperator<float64_t>* log_operator,
	SGVector<float64_t> vector)
	: CIndependentJob(aggregator)
{
	init();

	m_log_operator=log_operator;
	SG_REF(m_log_operator);

	m_vector=vector;

	SG_GCDEBUG("%s created (%p)\n", this->get_name(), this)
}

void CDenseExactLogJob::init()
{
	m_log_operator=NULL;

	SG_ADD((CSGObject**)&m_log_operator, "log_operator",
		"Log of linear operator", MS_NOT_AVAILABLE);

	SG_ADD(&m_vector, "trace_sample",
		"Sample vector to apply linear operator on", MS_NOT_AVAILABLE);
}

CDenseExactLogJob::~CDenseExactLogJob()
{
	SG_UNREF(m_log_operator);

	SG_GCDEBUG("%s destroyed (%p)\n", this->get_name(), this)
}

void CDenseExactLogJob::compute()
{
	SG_DEBUG("Entering...\n")

	REQUIRE(m_log_operator, "Log operator function is NULL\n");
	REQUIRE(m_aggregator, "Job result aggregator is NULL\n");

	// apply the log to m_vector
	SGVector<float64_t> vec=m_log_operator->apply(m_vector);

	// compute the vector-vector dot product using Eigen3
	Map<VectorXd> v(vec.vector, vec.vlen);
	Map<VectorXd> s(m_vector.vector, m_vector.vlen);

	CScalarResult<float64_t>* result=new CScalarResult<float64_t>(s.dot(v));
	SG_REF(result);

	m_aggregator->submit_result(result);
	SG_UNREF(result);

	SG_DEBUG("Leaving...\n")
}

SGVector<float64_t> CDenseExactLogJob::get_vector() const
{
	return m_vector;
}

CDenseMatrixOperator<float64_t>* CDenseExactLogJob::get_operator() const
{
	return m_log_operator;
}

}
