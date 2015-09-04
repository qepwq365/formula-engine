#include "stdafx.h"
#include "TokenPool.h"


unsigned TokenPool::AddToken(const std::string & token) {
	for(unsigned i = 0; i < m_pool.size(); ++i) {
		if(m_pool[i] == token)
			return i + 1;
	}

	m_pool.emplace_back(token);
	return m_pool.size();
}
