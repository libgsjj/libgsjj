#include "gsjj/passive/Method.h"

#include "MapleCOMSPS_LRB/utils/System.h"

#include "gsjj/passive/MethodFactory.h"

#include "gsjj/passive/SMT/BiermannMethod.h"
#include "gsjj/passive/SMT/NeiderJansenMethod.h"

#include "gsjj/passive/CNF/UnaryCNFMethod.h"
#include "gsjj/passive/CNF/BinaryCNFMethod.h"
#include "gsjj/passive/CNF/HeuleVerwerCNFMethod.h"

#include "gsjj/passive/NonCNF/UnaryNonCNFMethod.h"
#include "gsjj/passive/NonCNF/BinaryNonCNFMethod.h"
#include "gsjj/passive/NonCNF/HeuleVerwerNonCNFMethod.h"

#include "gsjj/passive/utils.h"

using namespace Minisat;

namespace gsjj {
    namespace passive {

        Method::Method(const std::set<std::string> &SpSet, const std::set<std::string> &SmSet, const std::set<std::string> &SSet, const std::set<std::string> &prefixesSet, const std::set<char> &alphabetSet, unsigned int n) :
            m_Sp(SpSet),
            m_Sm(SmSet),
            m_numberStates(n),
            m_S(SSet),
            m_prefixes(prefixesSet),
            m_alphabet(alphabetSet),
            m_triedSolve(false),
            m_cpuTimeStart(0),
            m_cpuTimeEnd(0),
            m_hasSolution(false)
        {

        }

        Method::~Method() {
        }

        unsigned int Method::numberOfStates() const {
            return m_numberStates;
        }

        double Method::timeToSolve() const {
            return m_cpuTimeEnd - m_cpuTimeStart;
        }
    }
}