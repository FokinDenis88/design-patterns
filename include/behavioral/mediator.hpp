#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace mediator {
			// https://en.wikipedia.org/wiki/Mediator_pattern
			// Motivation:

			class ColleagueAbstract;

			/** Abstract. */
			class IMediator {
			public:
				virtual ~IMediator() = default;

				virtual void Mediate() = 0;
				virtual void ColleagueChanged(ColleagueAbstract&) = 0;
			};

			class ConcreteMediator : public IMediator {
			public:
				void Mediate() override {
				};
				void ColleagueChanged(ColleagueAbstract&) override {

				}
			};

			/** Abstract. */
			class ColleagueAbstract {
			public:
				virtual ~ColleagueAbstract() = default;

				virtual void state_ptr() = 0;

			protected:
				inline std::unique_ptr<IMediator>& mediator() noexcept { return mediator_; };
				inline void set_mediator(std::unique_ptr<IMediator>&& mediator_p) noexcept { mediator_ = std::move(mediator_p); }

			private:
				std::unique_ptr<IMediator> mediator_;
			};

			class ConcreteColleague : public ColleagueAbstract{
			public:
				void state_ptr() override {
				};

				void Action1() {};
			};

			void Run();

		} // !namespace mediator
	} // !namespace behavioral
} // !namespace pattern

#endif // !MEDIATOR_HPP
