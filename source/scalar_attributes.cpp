#include "scalar_attributes.h"
#include <algorithm>

void ScalarAttributes::setC0Scalar(int i, float v) 
{ 
	m_values[i] = v; 
	m_minValue = std::min(v, m_minValue);
	m_maxValue = std::max(v, m_maxValue);
}