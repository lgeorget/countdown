#include <algorithm>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cmath>
#include <set>

#include <vector>
#include <deque>
#include <string>

struct Derivation
{
	std::vector<std::string> ops{};
	std::vector<long> availables{};
	long result = 0;
};

bool operator==(const Derivation& d1, const Derivation& d2)
{
	return d1.availables == d2.availables;
}

bool operator<(const Derivation& d1, const Derivation& d2)
{
	return d1.availables < d2.availables;
}

std::ostream& operator<<(std::ostream& out, const Derivation& d)
{
	out << d.result << "\n";
	for (auto&& o : d.ops) {
		out << "\t" << o << "\n";
	}
	return out;
}


int main()
{
	std::vector<long> numbers;
	long target;

	std::cin >> target;
	std::copy(std::istream_iterator<long>(std::cin), std::istream_iterator<long>{}, std::back_inserter(numbers));

	std::cout << "Numbers: ";
	std::copy(numbers.begin(), numbers.end(), std::ostream_iterator<long>{std::cout, " "});
	std::cout << "\n";

	std::cout << "Target: " << target << std::endl;

	std::sort(numbers.begin(), numbers.end());
	std::vector<Derivation> derivations{ Derivation{std::vector<std::string>{}, numbers, 0} };
	std::vector<Derivation> results;
	unsigned long closest = target;

	unsigned long rounds = 1;
	unsigned long maxDerivations = 1;

	while (!derivations.empty()) {
		Derivation d = derivations.back();
		derivations.pop_back();
		if (d.availables.size() == 1) {
			if (std::abs(target - d.availables.front()) < closest) {
				results.clear();
			}
			if (std::abs(target - d.availables.front()) <= closest) {
				results.push_back(Derivation{ d.ops, d.availables, d.availables.front() });
				closest = std::abs(target - d.availables.front());
			}
		} else {
			for (auto it = d.availables.begin() ; it != d.availables.end() ; ++it) {
				for (auto jt = std::next(it) ; jt != d.availables.end() ; ++jt) {
					std::vector<long> ongoing;
					for (auto copyIt = d.availables.begin() ; copyIt != d.availables.end() ; ++copyIt) {
						if (copyIt != it && copyIt != jt)
							ongoing.push_back(*copyIt);
					}

					long first = *it;
					long second = *jt;

					if (second > first) {
						std::swap(first, second);
					}

					auto addDerivation = [&derivations, &ongoing, &results, target, &closest](const Derivation& base, std::string&& op, long newElt) {
						Derivation result{base.ops, {}, base.result};
						result.ops.push_back(op);
						result.availables.reserve(ongoing.size() + 1);
						auto it = ongoing.begin();
						while (it != ongoing.end() && newElt > *it) {
							result.availables.push_back(*it);
							++it;
						}
						result.availables.push_back(newElt);
						while (it != ongoing.end()) {
							result.availables.push_back(*it);
							++it;
						}

						if (std::abs(target - newElt) < closest) {
							results.clear();
						}
						if (std::abs(target - newElt) <= closest) {
							result.result = newElt;
							results.push_back(result);
							closest = std::abs(target - newElt);
						}
						derivations.push_back(result);
					};

					long sum = first + second;
					std::ostringstream out;
					out << first << " + " << second << " = " << sum;
					addDerivation(d, out.str(), sum);

					if (first != 1 && second != 1) {
						long product = first * second;
						out = std::ostringstream{};
						out << first << " * " << second << " = " << product;
						addDerivation(d, out.str(), product);
					}

					if (first != second) {
						long sub = first - second;
						out = std::ostringstream{};
						out << first << " - " << second << " = " << sub;
						addDerivation(d, out.str(), sub);
					}

					if (first % second == 0 && second != 1) {
						long div = first / second;
						out = std::ostringstream{};
						out << first << " / " << second << " = " << div;
						addDerivation(d, out.str(), div);
					}
				}
			}
		}

		if (rounds % 10'000 == 0) {
			std::cerr << "After round " << rounds << ", " << derivations.size() << " derivations"
				<< ", best solution is " << closest << " away from target"
				<< std::endl;
		}

		maxDerivations = std::max(maxDerivations, derivations.size());
		rounds++;
	};

	std::cerr << results.size() << " raw solution(s)" << std::endl;
	for (auto&& r : results) {
		std::cerr << "Result: " << r.result << " (" << std::abs(target - r.result) << ")\n"
				  << r << "\n";
	}

	std::cerr << "Removing duplicates..." << std::endl;
	std::sort(results.begin(), results.end(), [](const Derivation& d1, const Derivation& d2) {
		return d1.ops < d2.ops;
	});
	results.erase(std::unique(results.begin(), results.end(), [](const Derivation& d1, const Derivation& d2) {
		return std::is_permutation(d1.ops.begin(), d1.ops.end(), d2.ops.begin(), d2.ops.end());
	}), results.end());
	std::cout << results.size() << " solution(s)\n"
		<< "max number of derivations: " << maxDerivations << "\n"
		<< "number of rounds: " << rounds << "\n";
	for (auto&& r : results) {
		std::cout << "Result: " << r.result << " (" << std::abs(target - r.result) << ")\n"
				  << r << "\n";
	}
}
