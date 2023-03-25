//
// Copyright (c) 2019-2023 yanggaofeng
//

#include <yangstream/YangSynBufferManager.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>


yang_vector_impl2(YangSynBuffer)

int32_t yang_synBufMan_getAudioDatas(YangSynBufferSession* session, uint8_t* mixBuf,YangFrame* pframe){
	if(session==NULL || pframe==NULL) return 1;

	if(session->playBuffers==NULL) return 1;
	uint8_t *tmp = NULL;
	int32_t count=0;
	yang_thread_mutex_lock(&session->playBufferMutex);
	YangSynBuffer* syn;
	for (int32_t i = 0; i < session->playBuffers->vec.vsize; i++) {
		syn=session->playBuffers->vec.payload[i];
		if (syn && syn->getAudioSize(syn->session) > 0) {
			tmp = syn->getAudioRef(syn->session,pframe);

			if (tmp) {
				// if (m_preProcess)	m_preProcess->preprocess_run((short*) tmp);
				if (count == 0) {
					yang_memcpy(mixBuf, tmp, pframe->nb);
				} else {
					yang_mixaudio_mix1((short*)mixBuf, (short*)tmp, pframe->nb, 128);
				}
				count++;
			}
			tmp = NULL;

			// if (m_in_audioBuffer->at(i)->size() > m_audioPlayCacheNum)                m_in_audioBuffer->at(i)->resetIndex();
		}
	}
	yang_thread_mutex_unlock(&session->playBufferMutex);


	return count>0?Yang_Ok:1;
}

int32_t yang_synBufMan_getPlayBufferIndex(YangSynBufferSession* session,int puid){
	if(session==NULL) return -1;

	if(session->playBuffers){
		for(int32_t i=0;i<session->playBuffers->vec.vsize;i++){
			if(session->playBuffers->vec.payload[i]->getUid(session->playBuffers->vec.payload[i]->session)==puid) return i;
		}
	}
	return -1;
}


int32_t yang_synBufMan_addAudioBuffer(YangSynBufferSession* session,void* yangAudioPlay,int32_t puid){
	if(session==NULL) return 1;
	YangAudioPlayBuffer* audioPlayBuf=(YangAudioPlayBuffer*)yangAudioPlay;
	int ind=yang_synBufMan_getPlayBufferIndex(session,puid);
	if(ind==-1){
		YangSynBuffer* syn=(YangSynBuffer*)yang_calloc(sizeof(YangSynBuffer),1);//new YangSynBuffer();
		syn->setUid(syn->session,puid);
		syn->setInAudioBuffer(syn->session,audioPlayBuf);
		yang_thread_mutex_lock(&session->playBufferMutex);
		if(session->playBuffers) session->playBuffers->insert(&session->playBuffers->vec,syn);
		yang_thread_mutex_unlock(&session->playBufferMutex);
		return Yang_Ok;
	}
	return 1;
}

void yang_synBufMan_initAudioBuffer(YangSynBufferSession* session){
	if(session==NULL||session->playBuffers!=NULL) return;
	session->playBuffers=(YangSynBufferVector2*)yang_calloc(sizeof(YangSynBufferVector2),1);
	yang_create_YangSynBufferVector2(session->playBuffers);
}

void yang_synBufMan_removeAllAudioBuffer(YangSynBufferSession* session){
	if(session==NULL) return ;
	yang_thread_mutex_lock(&session->playBufferMutex);
	if(session->playBuffers){
		for(int32_t i=0;i<session->playBuffers->vec.vsize;i++){
			yang_free(session->playBuffers->vec.payload[i]);
		}
		session->playBuffers->clear(&session->playBuffers->vec);
	}
	yang_thread_mutex_unlock(&session->playBufferMutex);
}

YangSynBuffer* yang_synBufMan_getSynBuffer(YangSynBufferSession* session,int puid){
	if(session==NULL) return NULL;
	int ind=yang_synBufMan_getPlayBufferIndex(session,puid);
	if(ind==-1) return NULL;
	return session->playBuffers->vec.payload[ind];
}

void yang_synBufMan_removeAudioBuffer(YangSynBufferSession* session,int32_t puid){
	if(session==NULL) return ;
	yang_thread_mutex_lock(&session->playBufferMutex);
	int ind=yang_synBufMan_getPlayBufferIndex(session,puid);
	if(session->playBuffers&&ind>-1){
		yang_destroy_synBuffer(session->playBuffers->vec.payload[ind]);
		yang_free(session->playBuffers->vec.payload[ind]);
		session->playBuffers->remove(&session->playBuffers->vec, ind);
	}
	yang_thread_mutex_unlock(&session->playBufferMutex);

}


void yang_create_synBufferManager(YangSynBufferManager* man){
	if(man==NULL) return;
	YangSynBufferSession* session=(YangSynBufferSession*)yang_calloc(sizeof(YangSynBufferSession),1);
	man->session=session;
	yang_thread_mutex_init(&session->playBufferMutex,NULL);

	man->getSynBuffer=yang_synBufMan_getSynBuffer;
	man->getPlayBufferIndex=yang_synBufMan_getPlayBufferIndex;
	man->getAudioDatas=yang_synBufMan_getAudioDatas;
	man->addAudioBuffer=yang_synBufMan_addAudioBuffer;
	man->initAudioBuffer = yang_synBufMan_initAudioBuffer;
	man->removeAllAudioBuffer=yang_synBufMan_removeAllAudioBuffer;
	man->removeAudioBuffer=yang_synBufMan_removeAudioBuffer;

}

void yang_destroy_synBufferManager(YangSynBufferManager* man){
	if(man==NULL||man->session==NULL) return;
	YangSynBufferSession* session=(YangSynBufferSession*)man->session;
	yang_free(session->playBuffer);
	if(session->playBuffers){
		for(int32_t i=0;i<session->playBuffers->vec.vsize;i++){
			yang_destroy_synBuffer(session->playBuffers->vec.payload[i]);
			yang_free(session->playBuffers->vec.payload[i]);
		}
		session->playBuffers->clear(&session->playBuffers->vec);
		yang_destroy_YangSynBufferVector2(session->playBuffers);
		yang_free(session->playBuffers);
	}

	yang_thread_mutex_destroy(&session->playBufferMutex);
}
