/******************************************************************************
 * Spine Runtimes Software License
 * Version 2.3
 * 
 * Copyright (c) 2013-2015, Esoteric Software
 * All rights reserved.
 * 
 * You are granted a perpetual, non-exclusive, non-sublicensable and
 * non-transferable license to use, install, execute and perform the Spine
 * Runtimes Software (the "Software") and derivative works solely for personal
 * or internal use. Without the written permission of Esoteric Software (see
 * Section 2 of the Spine Software License Agreement), you may not (a) modify,
 * translate, adapt or otherwise create derivative works, improvements of the
 * Software or develop new applications using the Software or (b) remove,
 * delete, alter or obscure any trademarks or any copyright, trademark, patent
 * or other intellectual property or proprietary rights notices on or in the
 * Software, including any copy thereof. Redistributions in binary or source
 * form must include this license and terms.
 * 
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ESOTERIC SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <spine/WeightedMeshAttachment.h>
#include <spine/extension.h>

void _spWeightedMeshAttachment_dispose (spAttachment* attachment) {
	spWeightedMeshAttachment* self = SUB_CAST(spWeightedMeshAttachment, attachment);
	_spAttachment_deinit(attachment);
	FREE(self->path);
	FREE(self->uvs);
	if (!self->parentMesh) {
		FREE(self->regionUVs);
		FREE(self->bones);
		FREE(self->weights);
		FREE(self->triangles);
		FREE(self->edges);
	}
	FREE(self);
}

spWeightedMeshAttachment* spWeightedMeshAttachment_create (const char* name) {
	spWeightedMeshAttachment* self = NEW(spWeightedMeshAttachment);
	self->r = 1;
	self->g = 1;
	self->b = 1;
	self->a = 1;
	_spAttachment_init(SUPER(self), name, SP_ATTACHMENT_WEIGHTED_MESH, _spWeightedMeshAttachment_dispose);
	return self;
}

void spWeightedMeshAttachment_updateUVs (spWeightedMeshAttachment* self) {
    int i;
    float width = self->regionU2 - self->regionU, height = self->regionV2 - self->regionV;
    int verticesLength = self->worldVerticesLength;
    FREE(self->uvs);
    self->uvs = MALLOC(float, verticesLength);
    if (self->regionRotate) {
        for (i = 0; i < verticesLength; i += 2) {
            self->uvs[i] = self->regionU + self->regionUVs[i + 1] * width;
            self->uvs[i + 1] = self->regionV + height - self->regionUVs[i] * height;
        }
    } else {
        for (i = 0; i < verticesLength; i += 2) {
            self->uvs[i] = self->regionU + self->regionUVs[i] * width;
            self->uvs[i + 1] = self->regionV + self->regionUVs[i + 1] * height;
        }
    }
}

static void spVertexAttachment_computeWorldVertices1 (spWeightedMeshAttachment* self, int start, int count, spSlot* slot, float* worldVertices, int offset);

void spWeightedMeshAttachment_computeWorldVertices (spWeightedMeshAttachment* self, spSlot* slot, float* worldVertices) {
    spVertexAttachment_computeWorldVertices1(self, 0, self->worldVerticesLength, slot, worldVertices, 0);
    /*
	int w = 0, v = 0, b = 0, f = 0;
	float x = slot->bone->skeleton->x, y = slot->bone->skeleton->y;
	spBone** skeletonBones = slot->bone->skeleton->bones;
	if (slot->attachmentVerticesCount == 0) {
		for (; v < self->bonesCount; w += 2) {
			float wx = 0, wy = 0;
			const int nn = self->bones[v] + v;
			v++;
			for (; v <= nn; v++, b += 3) {
				const spBone* bone = skeletonBones[self->bones[v]];
				const float vx = self->weights[b], vy = self->weights[b + 1], weight = self->weights[b + 2];
				wx += (vx * bone->a + vy * bone->b + bone->worldX) * weight;
				wy += (vx * bone->c + vy * bone->d + bone->worldY) * weight;
			}
			worldVertices[w] = wx + x;
			worldVertices[w + 1] = wy + y;
		}
	} else {
		const float* ffd = slot->attachmentVertices;
		for (; v < self->bonesCount; w += 2) {
			float wx = 0, wy = 0;
			const int nn = self->bones[v] + v;
			v++;
			for (; v <= nn; v++, b += 3, f += 2) {
				const spBone* bone = skeletonBones[self->bones[v]];
				const float vx = self->weights[b] + ffd[f], vy = self->weights[b + 1] + ffd[f + 1], weight = self->weights[b + 2];
				wx += (vx * bone->a + vy * bone->b + bone->worldX) * weight;
				wy += (vx * bone->c + vy * bone->d + bone->worldY) * weight;
			}
			worldVertices[w] = wx + x;
			worldVertices[w + 1] = wy + y;
		}
	}
    */
}

void spVertexAttachment_computeWorldVertices1 (spWeightedMeshAttachment* self, int start, int count, spSlot* slot, float* worldVertices, int offset) {
    spSkeleton* skeleton;
    float x, y;
    int deformLength;
    float* deform;
    float* vertices;
    int* bones;

    count += offset;
    skeleton = slot->bone->skeleton;
    x = skeleton->x;
    y = skeleton->y;
    deformLength = slot->attachmentVerticesCount;
    deform = slot->attachmentVertices;
    vertices = self->weights;
    bones = self->bones;
    if (!bones) {
        spBone* bone;
        int v, w;
        if (deformLength > 0) vertices = deform;
        bone = slot->bone;
        x += bone->worldX;
        y += bone->worldY;
        for (v = start, w = offset; w < count; v += 2, w += 2) {
            float vx = vertices[v], vy = vertices[v + 1];
            worldVertices[w] = vx * bone->a + vy * bone->b + x;
            worldVertices[w + 1] = vx * bone->c + vy * bone->d + y;
        }
    } else {
        int v = 0, skip = 0, i;
        spBone** skeletonBones;
        for (i = 0; i < start; i += 2) {
            int n = bones[v];
            v += n + 1;
            skip += n;
        }
        skeletonBones = skeleton->bones;
        if (deformLength == 0) {
            int w, b;
            for (w = offset, b = skip * 3; w < count; w += 2) {
                float wx = x, wy = y;
                int n = bones[v++];
                n += v;
                for (; v < n; v++, b += 3) {
                    spBone* bone = skeletonBones[bones[v]];
                    float vx = vertices[b], vy = vertices[b + 1], weight = vertices[b + 2];
                    wx += (vx * bone->a + vy * bone->b + bone->worldX) * weight;
                    wy += (vx * bone->c + vy * bone->d + bone->worldY) * weight;
                }
                worldVertices[w] = wx;
                worldVertices[w + 1] = wy;
            }
        } else {
            int w, b, f;
            for (w = offset, b = skip * 3, f = skip << 1; w < count; w += 2) {
                float wx = x, wy = y;
                int n = bones[v++];
                n += v;
                for (; v < n; v++, b += 3, f += 2) {
                    spBone* bone = skeletonBones[bones[v]];
                    float vx = vertices[b] + deform[f], vy = vertices[b + 1] + deform[f + 1], weight = vertices[b + 2];
                    wx += (vx * bone->a + vy * bone->b + bone->worldX) * weight;
                    wy += (vx * bone->c + vy * bone->d + bone->worldY) * weight;
                }
                worldVertices[w] = wx;
                worldVertices[w + 1] = wy;
            }
        }
    }
}

void spWeightedMeshAttachment_setParentMesh (spWeightedMeshAttachment* self, spWeightedMeshAttachment* parentMesh) {
	CONST_CAST(spWeightedMeshAttachment*, self->parentMesh) = parentMesh;
	if (parentMesh) {
		self->bones = parentMesh->bones;
		self->bonesCount = parentMesh->bonesCount;

		self->weights = parentMesh->weights;
		self->weightsCount = parentMesh->weightsCount;

		self->regionUVs = parentMesh->regionUVs;
		self->worldVerticesLength = parentMesh->worldVerticesLength;

		self->triangles = parentMesh->triangles;
		self->trianglesCount = parentMesh->trianglesCount;

		self->hullLength = parentMesh->hullLength;

		self->edges = parentMesh->edges;
		self->edgesCount = parentMesh->edgesCount;

		self->width = parentMesh->width;
		self->height = parentMesh->height;
	}
}
