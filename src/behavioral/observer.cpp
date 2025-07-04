#include "behavioral/observer/observer-others.hpp"

namespace pattern {
	namespace behavioral {

		namespace observer_wiki_version {
			Observer::Observer(Subject& subj) : subject(subj) {
				subject.attach(*this);
			}

			Observer::~Observer() {
				subject.detach(*this);
			}
		}

	}
}
