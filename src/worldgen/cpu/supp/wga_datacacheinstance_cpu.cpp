#include "wga_datacacheinstance_cpu.h"

#include <iostream>

#include "util/iterators.h"
#include "util/containerutils.h"
#include "util/tracyutils.h"
#include  "worldgen/base/supp/wga_symbol.h"

std::shared_ptr<WGA_DataRecord_CPU> WGA_DataCacheInstance_CPU::get(const WGA_DataCacheInstance_CPU::Key &key) {
	Record &rec = hash_.valueRef(key);
	if(&rec == &hash_.defaultValue()) {
		auto &cnt = missCount_[key];
		cnt++;
		if(cnt > 2) {
			std::string msg = std::format("Key {} miss {}\n", key.symbol->description(), missCount_[key]);
			TracyMessage(msg.data(), msg.size());
			//std::cerr <<msg;
		}
		return {};
	}

	rec.lastAccess = accessCounter_++;
	return rec.value;
}

void WGA_DataCacheInstance_CPU::insert(const WGA_DataCacheInstance_CPU::Key &key, const std::shared_ptr<WGA_DataRecord_CPU> &value, size_t memorySize) {
	Record &rec = hash_[key];
	// If there already was a record with a given key, consider it deleted and reduce used memory
	usedMemory_ -= rec.memorySize;

	// Set up the new cache record
	rec.memorySize = memorySize;
	rec.lastAccess = accessCounter_++;
	rec.value = value;

	usedMemory_ += memorySize;

	while(usedMemory_ > memoryCapacity_) {
		ASSERT(!hash_.isEmpty());

		const auto scoreFunc = [ac = accessCounter_](const Record &rec) {
			return static_cast<float>(rec.memorySize) * (1 + 0.01 * static_cast<float>(rec.lastAccess - ac));
		};
		const Record r = hash_.takeIndex(hash_.probabilisticSelect(scoreFunc));
		usedMemory_ -= r.memorySize;
	}
}