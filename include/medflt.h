//
// Created by zhouy on 2019/12/27.
//

#ifndef TEST_MEDFLT_H
#define TEST_MEDFLT_H
#include <map>
#include <vector>
#include "numeric"

template <typename T> class TMedianFilter1D
{
    // Local Variables
private:
    // median filter aperture
    int m_windowsize;

    // output - filtered input
    std::vector<T> m_array;
    // histogram
    std::map<T, int> m_histogram;

    typedef typename std::map<T, int>::iterator iterator;
    // current median - position in tree
    iterator m_median;

public:

    // constructors
    TMedianFilter1D(): m_windowsize(3)
    {
    };

    TMedianFilter1D(int windowsize)
    {
        SetWindowSize(windowsize);
    };

private:
    // forbidden
    TMedianFilter1D(const TMedianFilter1D&);

public:
    // destructor
    ~TMedianFilter1D()
    {
    };

    // operators
    T& operator [](int m) { return m_array[m]; };
    const T& operator[](int m) const { return m_array[m]; };


    // selectors
    int Count() const { return m_array.size(); };
    bool IsEmpty() const { return m_array.empty(); };

    T getFilteredMean() const {return std::accumulate(m_array.begin(),m_array.end(),0.0)/m_array.size();}
    // modifiers

    void SetWindowSize(int windowsize)
    {
        if(windowsize < 3 || !(windowsize % 2))
            throw "Invalid window size";
        m_windowsize = windowsize;
    };


    void Execute(const std::vector<T>& v, bool enabled = true)
    {
        // clear output
        m_array.clear();
        // clear histogram
        m_histogram.erase(m_histogram.begin(), m_histogram.end());

        if(enabled)
        {
            // if filter is enabled - perform filtering
            FilterImpl(v);
        }
        else
        {
            // if filter is disabled - make simply copy of input
            m_array.insert(m_array.end(), v.begin(), v.end());
        }
    };

private:

    void Inc(const T& key)
    {
        typename std::map<T, int>::iterator it = m_histogram.find(key);
        if(it != m_histogram.end())
            it->second++;
        else
            m_histogram.insert(std::pair<T,int>(key, 1));
    };

    void Dec(const T& key)
    {
        typename std::map<T, int>::iterator it = m_histogram.find(key);
        if(it != m_histogram.end())
        {
            if(it->second == 1)
            {
                if(m_median != it)
                    m_histogram.erase(it);
                else
                    it->second = 0;
            }
            else
            {
                it->second--;
            }
        }
    };


    void FilterImpl(const std::vector<T>& obj)
    {
        T av, dv;
        int sum = 0, dl,  middle = m_windowsize/2+1;

        for (int j = -m_windowsize/2; j <= m_windowsize/2; j++ )
        {
            if ( j < 0 )
            {
                av = obj.front();
            }
            else if(j > obj.size()-1)
            {
                av = obj.back();
            }
            else
            {
                av = obj[j];
            }
            Inc(av);
        }


        for(m_median = m_histogram.begin(); m_median != m_histogram.end(); m_median++)
        {
            sum += m_median->second;
            if( sum >= middle )
                break;
        }

        m_array.push_back(m_median->first);
        dl = sum - m_median->second;

        int N = obj.size();
        for (int j = 1; j < N; j++)
        {
            int k = j - m_windowsize/2-1;
            dv = k < 0 ? obj.front() : obj[k];
            k = j + m_windowsize/2;
            av = k > (int)obj.size()-1 ? obj.back() : obj[k];
            if(av != dv)
            {
                Dec(dv);
                if(dv < m_median->first)
                    dl--;
                Inc(av);
                if( av < m_median->first )
                    dl++;
                if(dl >= middle)
                {
                    while(dl >= middle)
                    {
                        m_median--;
                        dl -= m_median->second;
                    }
                }
                else
                {
                    while (dl + m_median->second < middle)
                    {
                        dl += m_median->second;
                        m_median++;
                    }
                }
            }
            m_array.push_back(m_median->first);
        }
    };

};
#endif //TEST_MEDFLT_H
