@ctype mat4 hmm_mat4
@ctype vec3 hmm_vec3
@ctype vec4 hmm_vec4

@block skin_utils
void skin_pos_nrm(in vec4 pos, in vec4 nrm, in vec4 jweights, in uint jindices, out vec4 skin_pos, out vec4 skin_nrm) {
    const uint max_joints = 64;
    skin_pos = vec4(0, 0, 0, 1);
    skin_nrm = vec4(0, 0, 0, 0);
    vec4 weights = jweights / dot(jweights, vec4(1.0));
    vec4 xxxx, yyyy, zzzz;
    if (weights.x > 0) {
        uint jidx = (jindices & 255) * max_joints;
        xxxx = joint[jidx].xxxx;
        yyyy = joint[jidx].yyyy;
        zzzz = joint[jidx].zzzz;
        skin_pos.xyz += vec3(dot(pos, xxxx), dot(pos, yyyy), dot(pos, zzzz)) * weights.x;
        skin_nrm.xyz += vec3(dot(nrm, xxxx), dot(nrm, yyyy), dot(nrm, zzzz)) * weights.x;
    }
    if (weights.y > 0) {
        uint jidx = ((jindices >> 8) & 255) * max_joints;
        xxxx = joint[jidx].xxxx;
        yyyy = joint[jidx].yyyy;
        zzzz = joint[jidx].zzzz;
        skin_pos.xyz += vec3(dot(pos, xxxx), dot(pos, yyyy), dot(pos, zzzz)) * weights.y;
        skin_nrm.xyz += vec3(dot(nrm, xxxx), dot(nrm, yyyy), dot(nrm, zzzz)) * weights.y;
    }
    if (weights.z > 0) {
        uint jidx = ((jindices >> 16) & 255) * max_joints;
        xxxx = joint[jidx].xxxx;
        yyyy = joint[jidx].yyyy;
        zzzz = joint[jidx].zzzz;
        skin_pos.xyz += vec3(dot(pos, xxxx), dot(pos, yyyy), dot(pos, zzzz)) * weights.z;
        skin_nrm.xyz += vec3(dot(nrm, xxxx), dot(nrm, yyyy), dot(nrm, zzzz)) * weights.z;
    }
    if (weights.w > 0) {
        uint jidx = ((jindices >> 24) & 255) * max_joints;
        xxxx = joint[jidx].xxxx;
        yyyy = joint[jidx].yyyy;
        zzzz = joint[jidx].zzzz;
        skin_pos.xyz += vec3(dot(pos, xxxx), dot(pos, yyyy), dot(pos, zzzz)) * weights.w;
        skin_nrm.xyz += vec3(dot(nrm, xxxx), dot(nrm, yyyy), dot(nrm, zzzz)) * weights.w;
    }
    skin_pos.xyz = pos.xyz;
    skin_nrm.xyz = nrm.xyz;
}
@end

@vs vs
uniform vs_params {
    mat4 view_proj;
};

struct sb_vertex {
    vec3 pos;
    uint normal;
    uint joint_indices;
    uint joint_weights;
};

// FIXME: turn this into a matrix
struct sb_instance {
    vec4 xxxx;
    vec4 yyyy;
    vec4 zzzz;
};

// FIXME: turn this into a matrix
struct sb_joint {
    vec4 xxxx;
    vec4 yyyy;
    vec4 zzzz;
};

readonly buffer vertices {
    sb_vertex vtx[];
};

readonly buffer instances {
    sb_instance inst[];
};

readonly buffer joints {
    sb_joint joint[];
};

out vec3 color;

@include_block skin_utils

void main() {
    // skinned model-space position and normal
    vec4 in_pos = vec4(vtx[gl_VertexIndex].pos, 1.0);
    vec4 in_nrm = unpackSnorm4x8(vtx[gl_VertexIndex].normal);
    vec4 jweights = unpackUnorm4x8(vtx[gl_VertexIndex].joint_weights);
    uint jindices = vtx[gl_VertexIndex].joint_indices;
    vec4 pos, nrm;
    skin_pos_nrm(in_pos, in_nrm, jweights, jindices, pos, nrm);

    // transform pos and normal to world space
    vec4 ixxxx = inst[gl_InstanceIndex].xxxx;
    vec4 iyyyy = inst[gl_InstanceIndex].yyyy;
    vec4 izzzz = inst[gl_InstanceIndex].zzzz;
    pos = vec4(dot(pos, ixxxx), dot(pos, iyyyy), dot(pos, izzzz), 1.0);
    nrm = vec4(dot(nrm, ixxxx), dot(nrm, iyyyy), dot(nrm, izzzz), 0.0);

    gl_Position = view_proj * pos;
    color = (nrm.xyz + 1.0) * 0.5;
}
@end

@fs fs
in vec3 color;
out vec4 frag_color;

void main() {
    frag_color = vec4(color, 1.0);
}
@end

@program skinned vs fs
