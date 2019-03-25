#include "passive/Method.h"

#include "utils/System.h"

#include "passive/MethodFactory.h"

#include "passive/SMT/BiermannMethod.h"
#include "passive/SMT/NeiderJansenMethod.h"

#include "passive/CNF/UnaryCNFMethod.h"
#include "passive/CNF/BinaryCNFMethod.h"
#include "passive/CNF/HeuleVerwerCNFMethod.h"

#include "passive/NonCNF/UnaryNonCNFMethod.h"
#include "passive/NonCNF/BinaryNonCNFMethod.h"
#include "passive/NonCNF/HeuleVerwerNonCNFMethod.h"

#include "passive/utils.h"

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
            m_cpuTimeEnd(0)
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