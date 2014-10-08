/***********************************************
The MIT License (MIT)

Copyright (c) 2012 Athrun Arthur <athrunarthur@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*************************************************/
#ifndef FF_RUNTIME_RECORD_H_
#define FF_RUNTIME_RECORD_H_
#include "common/common.h"
#include "runtime/rtcmn.h"
#include "runtime/env.h"


namespace ff{
  namespace rt{
  struct record{
    enum{
      op_pop = 1,
      op_push = 2,
      op_steal = 3,
    };
    thrd_id_t   qid;
    thrd_id_t   op_tid;
    int op;
    int64_t h;
    int64_t t;
    int64_t rid;
    bool op_res;
    record(int64_t prid, thrd_id_t pqid, thrd_id_t pop_tid, int pp, int ph, int pt)
      : qid(pqid)
      , rid(prid)
      , op_tid(pop_tid)
      , op(pp)
      , h(ph)
      , t(pt){}
  };

  class all_records{
    public:
      ~all_records(){
        for(int i = 0; i < m_oRecords.size(); ++i)
        {
          delete m_oRecords[i];
        }
      }
      static all_records * getInstance(){
        if(s_pInstance == nullptr)
          s_pInstance = new all_records();
        return s_pInstance;
      }

      void      init(int thrd_num, const char * fp)
      {
#ifdef RECORD_WORK_STEAL 
        m_fp = fp;
        for(int i = 0; i <=thrd_num; ++i)
        {
                m_oRecords.push_back(new std::vector<record>());
                std::stringstream ss;
                ss<<i<<fp;
                std::ofstream file(ss.str(), std::ios::out | std::ios::binary);
                file.close();
        }
#endif 
      }

      void      add(record r)
      {
#ifdef RECORD_WORK_STEAL 
        thrd_id_t id = get_thrd_id();
        if(m_oRecords[id] == nullptr)
        {
          std::cout<<".";
          return;
        }
        if(m_oRecords[id]->size() >= 100000)
          write_to_file(id);
        m_oRecords[id]->push_back(r);
#endif
      }

      void      write_to_file(thrd_id_t id)
      {
#ifdef RECORD_WORK_STEAL 
        std::stringstream ss;
        ss<<id<<m_fp;
        std::ofstream myFile (ss.str(), std::ios::out | std::ios::app | std::ios::binary);
        myFile.write((const char *)(m_oRecords[id]->data()), sizeof(record) * m_oRecords[id]->size());
        m_oRecords[id]->clear();
        myFile.close();
#endif
      }

      void      dump_all()
      {
#ifdef RECORD_WORK_STEAL 
        for(int i = 0; i < m_oRecords.size(); ++i)
        {
          write_to_file(i);
        }
#endif
      }

    protected:
      all_records(){}
      const char * m_fp;
      static all_records * s_pInstance;
      typedef std::vector<record> RS;
      std::vector<RS *>         m_oRecords;
  };
  }//end namespace rt
}//end namespace ff
#endif
