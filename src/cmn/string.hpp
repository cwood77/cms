#ifndef ___cmn_string___
#define ___cmn_string___

#define WIN32_LEAN_AND_MEAN
#include <functional>
#include <set>
#include <string>
#include <windows.h>

namespace cmn {

inline std::wstring widen(const std::string& nstr)
{
   int len = nstr.size();
   std::wstring wstr(len + 1, 0);
   mbstowcs(&wstr[0], nstr.c_str(), len);
   return wstr.c_str();
}

inline std::string narrow(const std::wstring& wstr)
{
   int len = wstr.size();
   std::string nstr(len + 1, 0);
   wcstombs(&nstr[0], wstr.c_str(), len);
   return nstr.c_str();
}

template<class T>
inline std::basic_string<T> splitLast(const std::basic_string<T>& in, T delim, std::basic_string<T> *pFirst = NULL)
{
   auto pos = in.find_last_of(delim);
   if(pos == std::basic_string<T>::npos)
   {
      if(pFirst)
         *pFirst = in;
      return std::basic_string<T>();
   }
   else
   {
      if(pFirst)
         *pFirst = std::basic_string<T>(in.c_str(),pos);
      return std::basic_string<T>(in.c_str()+pos+1);
   }
}

inline std::wstring splitExt(const std::wstring& in, std::wstring *pFirst = NULL)
{
   return splitLast(in,L'.',pFirst);
}

inline std::wstring lower(const std::wstring& in)
{
   std::wstring copy(in.c_str());
   ::_wcslwr(const_cast<wchar_t*>(copy.c_str()));
   return copy;
}

template<class T>
inline void split(const std::basic_string<T>& in, T delim, std::function<void(const std::basic_string<T>&)> add)
{
   const T *pThumb = in.c_str();
   const T *pStart = pThumb;
   for(;;++pThumb)
   {
      if(*pThumb == delim || *pThumb == 0)
      {
         std::basic_string<T> word(pStart,pThumb-pStart);
         add(word);
         if(*pThumb != 0)
         {
            pThumb++;
            pStart = pThumb;
         }
         else
            break;
      }
   }
}

template<class T>
inline std::set<std::basic_string<T> > splitSet(const std::basic_string<T>& in, T delim)
{
   std::set<std::basic_string<T> > container;
   split<T>(in,delim,[&](auto& e){ container.insert(e); });
   return container;
}

} // namespace cmn

#endif // ___cmn_string___
